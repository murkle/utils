let readSamples = true;
import {
    Demodulator
}
from "./demodulator.js";
let button = document.querySelector("button");
let introSection = document.querySelector('.intro');
let msgString = '';
let msgsArray = [];
let started = false;
let msgReceived = false;

const demodulator = new Demodulator();

async function start() {
	
	// show map
	document.getElementById("map").removeAttribute("hidden");

	
    const sdr = await RtlSdr.requestDevice();
    introSection.style.display = "none";

    await sdr.open({
        ppm: 0.5
    });

    const actualSampleRate = await sdr.setSampleRate(2000000);
    const actualCenterFrequency = await sdr.setCenterFrequency(1090000000);

    await sdr.resetBuffer();
    while (readSamples) {
        if (!started) {
            console.log('starting...')
            started = true
        }

        // const samples = await sdr.readSamples(16 * 16384);
        const samples = await sdr.readSamples(128000);
        // console.log(samples)

        const data = new Uint8Array(samples);
        // console.log(data)

        demodulator.process(data, 256000, onMsg)
    }
}

const onMsg = (msg) => {
    if (!msgReceived) {
        msgReceived = true;
    }
    displayAircraftData(msg);
}

const mod = function (x, y) {

    return x - y * Math.floor(x / y);

}

const N_z = 15;
const NLconst = 1 - Math.cos(Math.PI / (2 * N_z));

const NL = function (lat) {

    if (lat == 87)
        return 2;
    if (lat == -87)
        return 2;
    if (lat > 87)
        return 1;
    if (lat < -87)
        return 1;

    var cos = Math.cos(Math.PI / 180 * lat);
    var b = cos * cos;
    var c = 1 - NLconst / b;
    var d = 2 * Math.PI / Math.acos(c);

    return Math.floor(d);

}

var planes = [];



var removeOldPlanes = function() {
	
	var now = new Date();
	
	for (var key in planes) {
		
		var plane = planes[key];
		
		// hide marker if no signal for over 1 min
		if (plane && !plane.hidden && plane.timestamp && (now - plane.timestamp) > 60 * 1000) {
			if (plane.marker) {
				console.log("hiding ", plane);
				plane.marker.setMap(null);
				plane.hidden = true;
			}
			
		}
  
}
	

	
}

var displayAircraftData = function (msg) {

    //var icao = msg.icao;
    var icaoId = "ICAO " + msg.icao;
    var divId;

    var plane = planes[icaoId];

    if (plane) {
        plane.update(msg);
		plane.timestamp = new Date();
        divId = document.getElementById(icaoId);

        // initially zero until enough data has come in
        if (plane.lng || plane.lat) {

            var gbGrid = os.Transform.fromLatLng({
                lat: plane.lat,
                lng: plane.lng
            });
            var info = "Longitude: " + plane.lng.toFixed(2) + " Latitude " + plane.lat.toFixed(2) + " Speed " + plane.speed.toFixed(2) + " Altitude " + plane.altitude + " Heading " + plane.heading.toFixed(2) + " Count " + plane.count + " OS Grid ref " + (gbGrid.ea) + " " + (gbGrid.no);

            if (!plane.marker) {

                var symbol = {
                    // Plane symbol
                    // Public Domain, https://commons.wikimedia.org/w/index.php?curid=1529037
					path: "M134.875,19.74c0.04-22.771,34.363-22.771,34.34,0.642v95.563L303,196.354v35.306l-133.144-43.821v71.424l30.813,24.072v27.923l-47.501-14.764l-47.501,14.764v-27.923l30.491-24.072v-71.424L3,231.66v-35.306l131.875-80.409V19.74z",
                    fillColor: '#0000FF',
                    fillOpacity: .5,
                    anchor: new google.maps.Point(152, 156),
                    strokeWeight: 0,
                    scale: .2,
                    rotation: plane.heading
                }

                plane.marker = new google.maps.Marker({
                    // The below line is equivalent to writing:
                    // position: new google.maps.LatLng(-34.397, 150.644)
                    position: {
                        lat: plane.lat,
                        lng: plane.lng
                    },
                    map: map,
					// heading=0.00000000 -> assume spurious data -> show as Pin not Plane
                    icon: plane.heading == 0 ? undefined : symbol
                });
                const infowindow = new google.maps.InfoWindow({
                    content: "<p>" + info + "</p>",
                });

                google.maps.event.addListener(plane.marker, "click", () => {
                    infowindow.open(map, plane.marker);
                });

            } else {

				// just in case it was hidden
				plane.marker.setMap(map);

                var latlng = new google.maps.LatLng(plane.lat, plane.lng);
                plane.marker.setPosition(latlng);

            }

            //divId.innerHTML = info;

        }

    } else {
        plane = new Aircraft(0);

        plane.update(msg);
        console.log("new plane", plane);
        planes[icaoId] = plane;

        divId = document.createElement("div");
        //divId.innerHTML = icaoId;

        document.body.append(divId);
        divId.id = icaoId;

    }

	removeOldPlanes();
}

button.onclick = () => start();
