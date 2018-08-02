/*
  Matrix library example for TM1637.
  
  Tested using modified TM1637 display  with 6x5 matrix
  Library based on TM1638 library by Ricardo Batista, adapted by Maxint-RD MMOLE 2018.

  Tested to work in the following environments:
    * Arduino Nano using Arduino IDE 1.8.2, Nano (Old Bootloader)), 4760 bytes flash, 344 bytes RAM ==> 4692/292

  Only compiled: not tested yet:
    * ATtiny44A using Arduino IDE 1.8.2 and ATTinyCore (8MHz, LTO enabled), 4016 bytes flash, 169 bytes RAM ==> barely fits! (due to Serial.print)
    * Board ATtiny Microcotrolers has no support for Serial, use ATTinyCore

  For more information see  https://github.com/maxint-rd/TM16xx
*/
#include <TM1637.h>
#include <TM16xxMatrix.h>

#if !defined(LED_BUILTIN)
  #define LED_BUILTIN 4
#endif

// Define a 4-digit display module. Pin suggestions:
// ESP8266 (Wemos D1): data pin 5 (D1), clock pin 4 (D2)
// ATtiny44A: data pin 9, clock pin 10 (LED_BUILTIN: 8)
// define a module on data pin 5 (D1), clock pin 4 (D2)
TM1637 module(5, 4);    //  DIO=5, CLK=4
#define MATRIX_NUMCOLUMNS 5
#define MATRIX_NUMROWS 6
TM16xxMatrix matrix(&module, MATRIX_NUMCOLUMNS, MATRIX_NUMROWS);    // TM16xx object, columns, rows

void setup()
{
  Serial.begin(115200);
  Serial.println(F("TM1637 matrix example"));
  Serial.println(F("TM1637: DIO=5, CLK=4"));
  Serial.println();
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(10);

  Serial.println(F("clear"));
  module.clearDisplay();

  // all at once
  Serial.println(F("all"));
  matrix.setAll(true);
  delay(200);

  // dim the display
  Serial.println(F("dim"));
  for(int j=0; j<3; j++)
  {
    for(int i=7; i>0; i--)
    {
      module.setupDisplay(true, i);
      delay(100);
    }
    for(int i=0; i<8; i++)
    {
      module.setupDisplay(true, i);
      delay(100);
    }
  }

  // Columns at once
  Serial.println(F("columns binary"));
  for(int i=0; i<MATRIX_NUMCOLUMNS; i++)
  {
    for(int j=0; j<=0xFF; j++)
    {
      matrix.setColumn(i,j);
      delay(5);
    }
    delay(200);
  }
  matrix.setAll(false);

  // One pixel, column by column
  Serial.println(F("one pixel by column"));
  for(int i=0; i<MATRIX_NUMCOLUMNS; i++)
  {
    for(int j=0; j<MATRIX_NUMROWS; j++)
    {
        matrix.setPixel(i,j, true);
        delay(50);
        matrix.setPixel(i,j, false);
    }
  }

  // One pixel, row by row
  Serial.println(F("one pixel by row"));
  for(int i=0; i<MATRIX_NUMROWS; i++)
  {
    for(int j=0; j<MATRIX_NUMCOLUMNS; j++)
    {
        matrix.setPixel(j,i, true);
        delay(50);
        matrix.setPixel(j,i, false);
    }
  }

  Serial.println(F("Setup done, loop (un)fills rows"));
  delay(500);
}

void loop()
{
  static bool fLoop=true;

  // Fill/unfill columns
  for(int i=0; i<MATRIX_NUMROWS; i++)
  {
    for(int j=0; j<MATRIX_NUMCOLUMNS; j++)
    {
        matrix.setPixel(j, i, fLoop);
        delay(100);
    }
  }
  fLoop=!fLoop;
  digitalWrite(LED_BUILTIN, fLoop);   // turn the LED on/off
}
