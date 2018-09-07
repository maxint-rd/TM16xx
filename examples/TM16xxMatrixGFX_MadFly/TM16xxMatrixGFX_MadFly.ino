/*
 * TM16xxMatrixGFX_MadFly
 * 
 * Arduino example sketch to demonstrate the Adafruit GFX library on a TM16xx driven LED Matrix
 * 
 * Based on MadFly.pde example of the MAX72xxPanel library
 * 
 * Confirmed to work in the following environments:
 *     - WeMos D1-mini and TM1640 8x8 MatrixLED Shield using Arduino IDE 1.6.10: DIN=D7/13/MOSI, CLK=D5/14/SCK, 253.144 bytes flash, 32.392 bytes RAM
 *     - Arduino Nano using Arduino IDE 1.8.2 (old bootloader), 10014 bytes flash, 213 bytes RAM 
 * 
 * For more information see  https://github.com/maxint-rd/TM16xx
 */

#include <Adafruit_GFX.h>
#include <TM1640.h>
#include <TM16xxMatrixGFX.h>

TM1640 module(2, 3);    // DIN=9, CLK=10
//TM1640 module(13, 14);    // For ESP8266/WeMos D1-mini: DIN=D7/13/MOSI, CLK=D5/14/SCK
#define MODULE_SIZECOLUMNS 16    // number of GRD lines, will be the y-height of the display
#define MODULE_SIZEROWS 8    // number of SEG lines, will be the x-width of the display
TM16xxMatrixGFX matrix(&module, MODULE_SIZECOLUMNS, MODULE_SIZEROWS);    // TM16xx object, columns, rows

int pinRandom = A0;

int wait = 20; // In milliseconds

void setup() {
  matrix.setIntensity(4); // Set brightness between 0 and 7
  randomSeed(analogRead(pinRandom)); // Initialize random generator
}

void loop() {
  static int x = matrix.width() / 2;
  static int y = matrix.height() / 2;
  int xNext, yNext;
  
  matrix.drawPixel(x, y, HIGH);
  matrix.write(); // Send bitmap to display

  delay(wait);

  matrix.drawPixel(x, y, LOW); // Erase the old position of our dot

  do {
    switch ( random(4) ) {
      case 0: xNext = constrain(x + 1, 0, matrix.width() - 1); yNext = y; break;
      case 1: xNext = constrain(x - 1, 0, matrix.width() - 1); yNext = y; break;
      case 2: yNext = constrain(y + 1, 0, matrix.height() - 1); xNext = x; break;
      case 3: yNext = constrain(y - 1, 0, matrix.height() - 1); xNext = x; break;
    }
  }
  while ( x == xNext && y == yNext ); // Repeat until we find a new coordinate

  x = xNext;
  y = yNext;
}
