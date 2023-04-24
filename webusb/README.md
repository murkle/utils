# WebUSB API

This project turns a device into a stateless machine which can be communicated with from any language in the browser using an API. For example you can change the colours of the LEDs or play a tune.

The initial implementation (and hence demo) uses Python (implemented with Skulpt)

No code is run on the device - code runs in the browser and the device is just controlled with the API (so the device always needs to be connected with a USB cable)

## Quickstart

1. Buy a suitable device eg Adafruit Circuit Playground Express and plug it into a USB port with a suitable micro-USB cable
2. Install Arduino IDE & also the libraries for the Circuit Playground Express and TinyUSB see [Using WebUSB with Arduino and TinyUSB](https://learn.adafruit.com/using-webusb-with-arduino-and-tinyusb/featured_products)
3. Go into Boards Manager (Ctrl-Shift-B) and delete all the Circuit Playground Express boards except Adafruit Circuit Playground Express SAMD21 (stops the wrong ones being autoselected)
4. Select Adafruit Circuit Playground Express SAMD21 board
5. Select Tools -> USB Stack -> TinyUSB
6. Download [webusb-api.ino](https://murkle.github.io/utils/webusb/webusb-api.ino) from this repository and open in Arduino IDE
7. Compile and upload it to your Circuit Playground Express
8. Open [web-usb.html](https://murkle.github.io/utils/webusb/web-usb.html) in Chrome
9. Click "Connect" and select Circuit Playground Express from the dialog
10. Type in and run a test program eg to play a tune `webusbNote(440,4)` or set all the LEDs to blue `webusb("#0000ff")`

These steps just need to be done once. After that it's as simple as plugging the device in, and clicking on the pop-up message from Chrome to open the URL
