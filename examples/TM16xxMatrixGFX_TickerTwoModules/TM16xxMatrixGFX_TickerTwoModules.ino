/*
 * TM16xxMatrixGFX_TickerTwoModules
 * 
 * Arduino example sketch to demonstrate the Adafruit GFX library on a TM16xx driven LED Matrix
 * 
 * Based on Ticker.pde example of the MAX72xxPanel library
 * 
 * Confirmed to work in the following environments:
 *     - WeMos D1-mini and double TM1640 based 8x16 LED Matrix using Arduino IDE 1.6.10: DIN=D7/13/MOSI, CLK=D5/14/SCK, 256.984 bytes flash, 32.760 bytes RAM, Free mem: 44816
 * 
 * For more information see  https://github.com/maxint-rd/TM16xx
 */

#include <Adafruit_GFX.h>
#include <TM1640.h>
#include <TM16xxMatrixGFX.h>

// Define the chip specific matrix-display modules
//TM1640 module(9, 10);    // DIN=9, CLK=10
TM1640 module(D7, D5);    // For ESP8266/WeMos D1-mini: DIN=D7/13/MOSI, CLK=D5/14/SCK
TM1640 module2(D7, D6);   // For ESP8266/WeMos D1-mini: shared DIN=D7/13/MOSI, different CLK
//TM1640 module3(D7, D3);   // For ESP8266/WeMos D1-mini: shared DIN=D7/13/MOSI, different CLK
//TM1640 module4(D7, D2);   // For ESP8266/WeMos D1-mini: shared DIN=D7/13/MOSI, different CLK

// Put the modules in an array
TM16xx * modules[]={&module,&module2};      
//TM16xx * modules[]={&module,&module2, &module3, &module4};    // more modules can be added

// Specify the size of each module and how they are combined
#define MODULE_SIZECOLUMNS 16   // number of GRD lines per module, will be the y-height of the display
#define MODULE_SIZEROWS 8       // number of SEG lines per module, will be the x-width of the display
//TM16xxMatrixGFX matrix(&module, MODULE_SIZECOLUMNS, MODULE_SIZEROWS);    // TM16xx object, columns, rows
TM16xxMatrixGFX matrix(modules, MODULE_SIZECOLUMNS, MODULE_SIZEROWS, 2, 1);    // TM16xx object, columns, rows, modulesCol, modulesRow

String tape = "Arduino";
int wait = 100; // In milliseconds

int spacer = 1;
int width = 5 + spacer; // The font width is 5 pixels

void setup()
{
  Serial.begin(115200);
  Serial.println("TM16xxMatrixGFX_TickerTwoModules");
  Serial.print("Free mem:");
  Serial.println(ESP.getFreeHeap());      // only for ESP8266

  matrix.setIntensity(1); // Use a value between 0 and 7 for brightness
  matrix.setRotation(1);
  matrix.setMirror(true);   // set X-mirror true when using the WeMOS D1 mini Matrix LED Shield (X0=Seg1/R1, Y0=GRD1/C1)

  Serial.println("print");
  matrix.fillScreen(LOW);
  matrix.println("1234567890ABCDEF");   // println() wraps 
  matrix.write(); // Send bitmap to display
  delay(1000);

  Serial.println("Setup done");
}

void loop()
{

  for ( int i = 0 ; i < width * tape.length() + matrix.width() - 1 - spacer; i++ ) {

    matrix.fillScreen(LOW);

    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height()-8)/2; // center the text vertically
    //int y = (i-8)%matrix.height(); // scroll up down too
    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < tape.length() ) {
        matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
      }

      letter--;
      x -= width;
    }
    matrix.drawPixel(0, 0, HIGH); // mark axis-origin for reference
    matrix.drawPixel(0, 3, HIGH);
    matrix.drawPixel(i%matrix.width(), 0, HIGH);
    //matrix.drawLine(0,i%matrix.height(), matrix.width(), matrix.height()-i%matrix.height()-1, HIGH);
    matrix.write(); // Send bitmap to display
    delay(wait);
  }
  Serial.print(".");
}

