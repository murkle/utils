/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 Copyright (c) 2019 Ha Thach for Adafruit Industries
 Copyright (c) 2023 GeoGebra GmbH
*********************************************************************/


#include "Adafruit_TinyUSB.h"
#include <Adafruit_NeoPixel.h>
#include <Adafruit_CircuitPlayground.h>

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
// use on-board neopixel PIN_NEOPIXEL if existed
#ifndef PIN_NEOPIXEL
  #define PIN_NEOPIXEL 8
#endif

// How many NeoPixels are attached to the Arduino?
// use on-board defined NEOPIXEL_NUM if existed
#ifndef NEOPIXEL_NUM
  #define NEOPIXEL_NUM  10
#endif

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NEOPIXEL_NUM, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

// USB WebUSB object
Adafruit_USBD_WebUSB usb_web;

// Landing Page: scheme (0: http, 1: https), url
// Page source can be found at https://github.com/hathach/tinyusb-webusb-page/tree/main/webusb-rgb
WEBUSB_URL_DEF(landingPage, 1 /*https*/, "test.geogebra.org/~mike/webusb/webusb-rgb_index.html");

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

  pixels.begin();
  pixels.setBrightness(10);
  pixels.fill(0xff0000);
  pixels.show();

  // wait until device mounted
  while( !TinyUSBDevice.mounted() ) delay(1);

  Serial.println("TinyUSB WebUSB RGB example");
  CircuitPlayground.begin();  // initialize the CP library
 
}

// convert a hex character to number
uint8_t char2num(char c)
{
  if (c >= 'a') return c - 'a' + 10;
  if (c >= 'A') return c - 'A' + 10;
  return c - '0';  
}

void loop()
{
  // Landing Page 7 characters as hex color '#RRGGBB'
  if (usb_web.available() < 7) return;

  uint8_t input[7];
  usb_web.readBytes(input, 7);

  // Print to serial for debugging
  Serial.write(input, 7);
  Serial.println();

   

  uint8_t red   = 16*char2num(input[1]) + char2num(input[2]);
  uint8_t green = 16*char2num(input[3]) + char2num(input[4]);
  uint8_t blue  = 16*char2num(input[5]) + char2num(input[6]);

  int tone = 256*char2num(input[1]) + 16*char2num(input[2]) + char2num(input[3]);
  int duration = 256*char2num(input[4]) + 16*char2num(input[5]) + char2num(input[6]);

  uint32_t color = (red << 16) | (green << 8) | blue;
  if (input[0] == '#') {
    pixels.fill(color);
  } else if (input[0] == 'b') {
    pixels.setBrightness(red);
    pixels.fill(0xffffff);
  } else if (input[0] == 'n') {
    CircuitPlayground.playTone(tone,1000/duration);
  } else {
    uint8_t pixel = char2num(input[0]);
    pixels.setPixelColor(pixel, color);
  }
  pixels.show();

 // syntaxes for pixels
 // CircuitPlayground.setPixelColor(4,   0,   0, 255);  
 // CircuitPlayground.setPixelColor(5, 0xFF0000);


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

// capacipive touch inputs, only works for A1-A6
// MRB: I can only get A4 and A5 to work with threshold of 200
 float capA1 = CircuitPlayground.readCap(A1);
 float capA2 = CircuitPlayground.readCap(A2);
 float capA3 = CircuitPlayground.readCap(A3);
 float capA4 = CircuitPlayground.readCap(A4); // 3
 float capA5 = CircuitPlayground.readCap(A5); // 2
 float capA6 = CircuitPlayground.readCap(A6);
 float capA7 = CircuitPlayground.readCap(A7);

// uint8_t buf[] = {left ? 1 : 0, right ? 1 : 0, slide ? 1 : 0, floor(x*256), floor(y*256), floor(z*256), capA1 < 200.0 ? 0 : 1,  capA2 < 200.0 ? 0 : 1, capA3 < 200.0 ? 0 : 1, capA4 < 200.0 ? 0 : 1, capA5 < 200.0 ? 0 : 1, capA6 < 200.0 ? 0 : 1, capA7 < 200.0 ? 0 : 1};
 //usb_web.write(buf, sizeof(buf));

sendString("{");
sendString("\"version\":1,");
sendString("\"leftButton\":");
sendString(left ? "1," : "0,");
sendString("\"rightButton\":");
sendString(right ? "1," : "0,");
 usb_web.flush();
sendString("\"temperature\":");
sendFloat(temp);
sendString(",");
sendString("\"sound\":");
sendFloat(sound);
sendString(",\"light\":");
sendString(String(light));
sendString("}");
 usb_web.flush();

   
}

void sendFloat(float f) {

  // TODO: maybe change to dtostrf() if this causes memory problems (likely!)
sendString(String(f));
}

void sendString(String text) {

char textArray[text.length() + 1];
  text.toCharArray(textArray, text.length() + 1);
  // -1 -> don't include null terminator!
usb_web.write(textArray, sizeof(textArray) - 1);

}

void line_state_callback(bool connected)
{
  // connected = green, disconnected = red
  pixels.fill(connected ? 0x00ff00 : 0xff0000);
  pixels.show();
}


