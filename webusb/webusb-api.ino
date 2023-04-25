/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 Copyright (c) 2019 Ha Thach for Adafruit Industries
 Copyright (c) 2023 GeoGebra GmbH
*********************************************************************/


#include "Adafruit_TinyUSB.h"
#include <Adafruit_CircuitPlayground.h>

// USB WebUSB object
Adafruit_USBD_WebUSB usb_web;

// Landing Page: scheme (0: http, 1: https), url
// Page source can be found at https://github.com/murkle/utils/blob/master/webusb/webusb-api.ino
WEBUSB_URL_DEF(landingPage, 1 /*https*/, "murkle.github.io/utils/webusb/web-usb.html");

// the setup function runs once when you press reset or power the board
void setup()
{
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
  TinyUSB_Device_Init(0);
#endif
  //usb_web.setStringDescriptor("TinyUSB WebUSB");
  usb_web.setLandingPage(&landingPage);
  usb_web.setLineStateCallback(line_state_callback);
  usb_web.begin();

  Serial.begin(115200);

  // This initializes the NeoPixel with RED
#ifdef NEOPIXEL_POWER
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, NEOPIXEL_POWER_ON);
#endif

  // wait until device mounted
  while( !TinyUSBDevice.mounted() ) delay(1);

  //Serial.println("TinyUSB WebUSB RGB example");
  CircuitPlayground.begin();  // initialize the CP library

   // Turn off all the NeoPixels (needed?)
  CircuitPlayground.strip.clear();
  CircuitPlayground.strip.show(); 
  CircuitPlayground.setPixelColor(1, 0x00ff00);
}

// convert a hex character to number
uint8_t char2num(char c)
{
  if (c >= 'a') return c - 'a' + 10;
  if (c >= 'A') return c - 'A' + 10;
  return c - '0';  
}

// command length eg #ff00ff has command = '#' and parametersLength = 6
int parametersLength(char command) {

  switch(command) {
    case 'v' : return 0;
    case 'b' : return 2;

    default: return 6;
  }
  return 6;
}

void loop()
{
  // Landing Page 7 characters as hex color '#RRGGBB'
  if (usb_web.available() < 1) return;

  // max possible parameters length
  uint8_t input[6];

  // read first byte (the command)
  usb_web.readBytes(input, 1);

// eg # for colour
// b for brightness 
// etc
  char command = input[0];

  int len = parametersLength(command);


    uint8_t parameters[len];

  if (len > 0) {
    usb_web.readBytes(parameters, len);
  }

  
  


  // Print to serial for debugging
  //Serial.write(input, 7);
  //Serial.println();

   


  if (command == '#') {
    uint8_t red   = 16*char2num(parameters[0]) + char2num(parameters[1]);
    uint8_t green = 16*char2num(parameters[2]) + char2num(parameters[3]);
    uint8_t blue  = 16*char2num(parameters[4]) + char2num(parameters[5]);
    uint32_t color = (red << 16) | (green << 8) | blue;
    for (int i = 0 ; i < 10 ; i++) {
       CircuitPlayground.setPixelColor(i, color);
    }
  } else if (command == 'b') {
        uint8_t bright   = 16*char2num(parameters[0]) + char2num(parameters[1]);

    CircuitPlayground.setBrightness(bright > 100 ? 100 : bright);
  } else if (command == 'n') {
    int tone = 256*char2num(parameters[0]) + 16*char2num(parameters[1]) + char2num(parameters[2]);
    int duration = 256*char2num(parameters[3]) + 16*char2num(parameters[4]) + char2num(parameters[5]);

    CircuitPlayground.playTone(tone,1000/duration);
  } else if (command == 'v') {
    // do nothing, will just return JSON status
  } else {
    // assume command is digit 0-9
    uint8_t red   = 16*char2num(parameters[0]) + char2num(parameters[1]);
    uint8_t green = 16*char2num(parameters[2]) + char2num(parameters[3]);
    uint8_t blue  = 16*char2num(parameters[4]) + char2num(parameters[5]);
    uint32_t color = (red << 16) | (green << 8) | blue;

    uint8_t pixel = char2num(command);
     CircuitPlayground.setPixelColor(pixel, color);
  }
 
// true/false
 boolean left = CircuitPlayground.leftButton();
 boolean right = CircuitPlayground.rightButton();
 boolean slide = CircuitPlayground.slideSwitch();

 // in m/s^2
  float x = CircuitPlayground.motionX();
  float y = CircuitPlayground.motionY();
  float z = CircuitPlayground.motionZ();
  float temp = CircuitPlayground.temperature();
  uint16_t light = CircuitPlayground.lightSensor();
 float sound = CircuitPlayground.mic.soundPressureLevel(10);

// capacitive touch inputs, only works for A1-A7
 float capA1 = CircuitPlayground.readCap(A1);
 float capA2 = CircuitPlayground.readCap(A2);
 float capA3 = CircuitPlayground.readCap(A3);
 float capA4 = CircuitPlayground.readCap(A4); // 3
 float capA5 = CircuitPlayground.readCap(A5); // 2
 float capA6 = CircuitPlayground.readCap(A6);
 float capA7 = CircuitPlayground.readCap(A7);

// build JSON status string to return
sendString("{");
sendString("\"apiVersion\":1,");
sendString("\"device\":\"Circuit Playground Express\",");
sendString("\"leftButton\":");
sendString(left ? "1," : "0,");
sendString("\"rightButton\":");
sendString(right ? "1," : "0,");
sendString("\"switch\":");
sendString(slide ? "1," : "0,");

// needs to be chunked into (max) 64 byte blocks
usb_web.flush();

sendString("\"A1\":");
sendFloat(capA1, true);
sendString(",\"A2\":");
sendFloat(capA2, true);
sendString(",\"A3\":");
sendFloat(capA3, true);
sendString(",\"A4\":");
sendFloat(capA4, true);
sendString(",\"A5\":");
sendFloat(capA5, true);
sendString(",\"A6\":");
sendFloat(capA6, true);
sendString(",\"A7\":");
sendFloat(capA7, true);

// needs to be chunked into (max) 64 byte blocks
usb_web.flush();

sendString(",\"temperature\":");
sendFloat(temp, false);
sendString(",\"sound\":");
sendFloat(sound, false);
sendString(",\"light\":");
sendString(String(light));
sendString("}");

// needs to be chunked into (max) 64 byte blocks
usb_web.flush();

   
}

void sendFloat(float f, boolean comma) {

  // TODO: maybe change to dtostrf() if this causes memory problems (likely!?)
sendString(String(f));
if (comma) {
  //sendString(",");
}
}

void sendString(String text) {

char textArray[text.length() + 1];
  text.toCharArray(textArray, text.length() + 1);
  // -1 -> don't include null terminator!
usb_web.write(textArray, sizeof(textArray) - 1);

}

void line_state_callback(bool connected)
{
  //
}


