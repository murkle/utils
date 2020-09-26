function require(url, callback) {
	var e = document.createElement("script");
	e.src = url;
	e.type = "text/javascript";
	e.addEventListener('load', callback);
	document.getElementsByTagName("head")[0].appendChild(e);
}
require("https://cdn.jsdelivr.net/gh/neocotic/qrious/dist/qrious.min.js", function() {
	require("https://murkle.github.io/utils/lightbox.js", function() {
		var qr = new QRious({
			background: 'white',
			backgroundAlpha: 0,
			foreground: 'black',
			foregroundAlpha: 1,
			level: 'H',
			padding: 25,
			size: 500,
			value: 'window.location.href'
		});
		openImageInLightBox(qr.image.src);
	});
});
