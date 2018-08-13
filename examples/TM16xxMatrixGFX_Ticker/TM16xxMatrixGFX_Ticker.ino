/*
 * TM16xxMatrixGFX_Ticker
 * 
 * Arduino example sketch to demonstrate the Adafruit GFX library on a TM16xx driven LED Matrix
 * 
 * Based on Ticker.pde example of the MAX72xxPanel library
 * 
 * Confirmed to work in the following environments:
 *     - WeMos D1-mini and TM1640 8x8 MatrixLED Shield using Arduino IDE 1.6.10: DIN=D7/13/MOSI, CLK=D5/14/SCK, 253.244 bytes flash, 32.388 bytes RAM
 * 
 */

#include <Adafruit_GFX.h>
#include <TM1640.h>
#include <TM16xxMatrixGFX.h>

//int pinCS = 10; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 1;
int numberOfVerticalDisplays = 1;

//Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
//TM1640 module(9, 10);    // DIN=9, CLK=10
TM1640 module(13, 14);    // For ESP8266/WeMos D1-mini: DIN=D7/13/MOSI, CLK=D5/14/SCK
#define MATRIX_NUMCOLUMNS 8
#define MATRIX_NUMROWS 8
TM16xxMatrixGFX matrix(&module, MATRIX_NUMCOLUMNS, MATRIX_NUMROWS);    // TM16xx object, columns, rows


String tape = "Arduino";
int wait = 100; // In milliseconds

int spacer = 1;
int width = 5 + spacer; // The font width is 5 pixels

void setup()
{

  matrix.setIntensity(1); // Use a value between 0 and 7 for brightness
  matrix.setRotation(0);

// Adjust to your own needs
//  matrix.setPosition(0, 0, 0); // The first display is at <0, 0>
//  matrix.setPosition(1, 1, 0); // The second display is at <1, 0>
//  matrix.setPosition(2, 2, 0); // The third display is at <2, 0>
//  matrix.setPosition(3, 3, 0); // And the last display is at <3, 0>
//  ...
//  matrix.setRotation(0, 2);    // The first display is position upside down
//  matrix.setRotation(3, 2);    // The same hold for the last display
}

void loop()
{

  for ( int i = 0 ; i < width * tape.length() + matrix.width() - 1 - spacer; i++ ) {

    matrix.fillScreen(LOW);

    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically

    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < tape.length() ) {
        matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
      }

      letter--;
      x -= width;
    }

    matrix.write(); // Send bitmap to display

    delay(wait);
  }
}

