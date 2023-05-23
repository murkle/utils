/*
https://www.amazon.co.uk/gp/product/B075FYYLLV/
DS18B20 library https://how2electronics.com/interfacing-ds18b20-sensor-with-raspberry-pi-pico/
https://github.com/pstolarz/OneWireNg
https://github.com/pstolarz/Arduino-Temperature-Control-Library/tree/OneWireNg

To upload your first sketch, you will need to hold the BOOTSEL button down while plugging in the Pico to your computer.
Then hit the upload button and the sketch should be transferred and start to run.
*/

#include <OneWire.h>
#include <OneWireNg.h>
#include <OneWireNg_BitBang.h>
#include <OneWireNg_Config.h>
#include <OneWireNg_CurrentPlatform.h>


#include <DallasTemperature.h>

#include "Adafruit_TinyUSB.h"

// GP9 Yellow pin of Stemma QT on Banana Pi
#define ONE_WIRE_BUS 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


Adafruit_USBD_WebUSB usb_web;
// define the URL where the system notification should point to
WEBUSB_URL_DEF(landingPage, 1, "murkle.github.io/utils/webusb/web-usb.html");

// default button state
byte prevButtonState = LOW;

void setup() {

  Serial.begin(9600);


#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // required for the RP2040 chip
  TinyUSB_Device_Init(0);
#endif

  usb_web.setLandingPage(&landingPage);
  usb_web.begin();

  // I'm using the builtin led on the board for a visual feedback. So I set the pinMode here as well
  pinMode(LED_BUILTIN, OUTPUT);
  // flash onboard LED
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);

  // wait until device mounted
  while (!TinyUSBDevice.mounted()) delay(1);

  // Start up the sensor OneWire library
  sensors.begin();
}

void loop() {



  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sendString("Requesting temperatures...\n");
  sensors.requestTemperatures();  // Send the command to get temperatures
  sendString("DONE\n");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C) {
    // send result as JSON
    sendStringRaw("{\"device\":\"Pico Temperature Sensor\", \"temperatureC\":");
    sendStringRaw(String(tempC));
    sendString("}\n");
  } else {
    sendString("Error: Could not read temperature data\n");
  }
}

void sendString(String text) {

  sendStringRaw(text);
  usb_web.flush();
}

void sendStringRaw(String text) {

  Serial.println(text);

  char textArray[text.length() + 1];
  text.toCharArray(textArray, text.length() + 1);
  // -1 -> don't include null terminator!
  usb_web.write(textArray, sizeof(textArray) - 1);
}
