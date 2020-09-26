function closeLightbox() {
    var div = document.getElementById("myLightboxDiv");
    if (div) {
        document.body.removeChild(div);
    }
}

function openImageInLightBox(imageBase64) {

    closeLightbox();

    div = document.createElement("div");
    div.id = "myLightboxDiv";
    div.style.width = "80%";
    div.style.height = "80%";
    div.style.position = "fixed";
    div.style.top = "10%";
    div.style.left = "10%";
    div.style.border = "7px solid rgba(0, 0, 0, 0.5)";
    div.style.background = "#FFF";
    div.style["background-image"] = "url('" + imageBase64 + "')";
    div.style["background-size"] = "contain";
    div.style["background-repeat"] = "no-repeat";
    div.style["background-position"] = "center";
    div.style.zIndex = 10000;

    div.onclick = function() {
        document.body.removeChild(div);
    };

    document.body.appendChild(div);

    // now add transparent image over it
    // so that "Save image as..." works
    // remove this if you don't need it
    var elem = document.createElement("img");
    elem.src = imageBase64;
    elem.style.height = "100%";
    elem.style.width = "100%";
    elem.style.opacity = 0;
    div.appendChild(elem);

}

function openPdfInLightBox(pdf) {

    closeLightbox();

    div = document.createElement("div");
    div.id = "myLightboxDiv";
    div.style.width = "80%";
    div.style.height = "80%";
    div.style.position = "fixed";
    div.style.top = "10%";
    div.style.left = "10%";
    div.style.border = "7px solid rgba(0, 0, 0, 0.5)";
    div.style.background = "#FFF";
    div.style.zIndex = 10000;

    div.onclick = function() {
        document.body.removeChild(div);
    };

    document.body.appendChild(div);

    var elem = document.createElement("iframe");
    elem.src = pdf;
    elem.style.position = "relative";
    elem.style.height = "100%";
    elem.style.width = "100%";
    div.appendChild(elem);

}