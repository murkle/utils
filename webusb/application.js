/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license
 Copyright (c) 2019 Adafruit Industries
 Adapted from https://adafruit.github.io/Adafruit_TinyUSB_Arduino/examples/webusb-rgb/application.js
*********************************************************************/

(function() {
  'use strict';

  document.addEventListener('DOMContentLoaded', event => {
    let connectButton = document.querySelector("#connect");
    let statusDisplay = document.querySelector('#status');
	
	var buffer = "";

    function connect() {
      port.connect().then(() => {
        statusDisplay.textContent = '';
        connectButton.textContent = 'Disconnect';
		
		 port.onReceive = data => {
          let textDecoder = new TextDecoder();
		  var jsonPart = textDecoder.decode(data);
		  console.log(jsonPart);
		  
		  if (jsonPart.startsWith("{")) {
			buffer = "";
		  }
		  
		  // data comes in (max) 64-byte chunks
		  buffer += jsonPart;
		  if (buffer.trim().endsWith("}")) {
			  var json = JSON.parse(buffer.trim());
			  console.log(json);
			  //console.log("version", json.apiVersion);
			  //console.log("leftButton", json.leftButton);
			  buffer = "";
			  
		  } 
		  
		  
		  
		   //var ints = new Int8Array(12);
           //  for (var i = 0; i < ints.length; i++) {
           //      ints[i] = data.getInt8(i);
           //  }
          //console.log("From USB: ", ints, textDecoder.decode(data));
          
        };

        port.onReceiveError = error => {
          console.error(error);
        };
      }, error => {
        statusDisplay.textContent = error;
      });
    }

    connectButton.addEventListener('click', function() {
      if (port) {
        port.disconnect();
        connectButton.textContent = 'Connect';
        statusDisplay.textContent = '';
        port = null;
      } else {
        serial.requestPort().then(selectedPort => {
          port = selectedPort;
          connect();
        }).catch(error => {
          statusDisplay.textContent = error;
        });
      }
    });

    serial.getPorts().then(ports => {
      if (ports.length === 0) {
        statusDisplay.textContent = 'No device found.';
      } else {
        statusDisplay.textContent = 'Connecting...';
        port = ports[0];
        connect();
      }
    });


  });
})();
