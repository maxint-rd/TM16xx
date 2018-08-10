/*
  Matrix library example for TM1640. Kept small to fit in the limited space of an ATtiny44.
  NOTE: compile using LTO enabled!

  Library based on TM1638 library by Ricardo Batista, adapted by Maxint-RD MMOLE 2018.
  Confirmed to work in the following environments:
    * ATtiny44A using Arduino IDE 1.8.2 and ATTinyCore (8MHz, LTO enabled), 3232 bytes flash, 103 bytes RAM ==> 3086/137 ==> 2996/141 ==> 3000/115
    * ATtiny44A using Arduino IDE 1.8.2 and ATtiny Microcontrolers (8MHz), 3212 bytes flash, 103 bytes RAM  == 3056/137 ==> 2976/115
    * WeMos D1-mini and TM1640 8x8 MatrixLED Shield using Arduino IDE 1.6.10: DIN=D7/13/MOSI, CLK=D5/14/SCK, 248644 bytes flash, 32312 bytes RAM

  Only compiled: not tested yet:
    * Arduino Nano using Arduino IDE 1.8.2, Nano (Old Bootloader)), 3388 bytes flash, 119 bytes RAM => 3298/115

  For more information see  https://github.com/maxint-rd/TM16xx
*/
#include <TM1640.h>
#include <TM16xxMatrix.h>

#if !defined(LED_BUILTIN)
  #define LED_BUILTIN 4
#endif

// Define a 4-digit display module. Pin suggestions:
// ESP8266 (Wemos D1): data pin 5 (D1), clock pin 4 (D2)
// ATtiny44A: data pin 9, clock pin 10 (LED_BUILTIN: 8 in ATtiny Core)
//TM1640 module(9, 10, 4);    // data, clock, 4 digits
TM1640 module(13, 14);    // For ESP8266/WeMos D1-mini: DIN=D7/13/MOSI, CLK=D5/14/SCK
#define MATRIX_NUMCOLUMNS 8
#define MATRIX_NUMROWS 8
TM16xxMatrix matrix(&module, MATRIX_NUMCOLUMNS, MATRIX_NUMROWS);    // TM16xx object, columns, rows

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  module.setDisplayToString("HALO");
  delay(500);                        // wait
  module.clearDisplay();

  // all at once
  matrix.setAll(true);
  delay(200);

  // Columns binary
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
  for(int i=0; i<MATRIX_NUMROWS; i++)
  {
    for(int j=0; j<MATRIX_NUMCOLUMNS; j++)
    {
        matrix.setPixel(j,i, true);
        delay(50);
        matrix.setPixel(j,i, false);
    }
  }
}

void loop()
{
/*
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  module.setupDisplay(true, 7);   // sometimes noise on the line may change the intensity level
  int nTime=((millis()/1000)/60)*100+(millis()/1000)%60;    // minutes+seconds as integer
  module.setDisplayToDecNumber(nTime, _BV(4)); // display dot on digit 4
  delay(500);                        // wait
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  module.setDisplayToDecNumber(nTime, _BV(3)); // display colon on digit 3
  delay(500);                        // wait
*/
  static bool fLoop=true;

  // Fill/unfill columns
  for(int i=0; i<MATRIX_NUMROWS; i++)
  {
    for(int j=0; j<MATRIX_NUMCOLUMNS; j++)
    {
        matrix.setPixel(j, i, fLoop);
        delay(50);
    }
/*
    for(int j=0; j<8; j++)
    {
        matrix.setPixel(i,j, false);
        delay(50);
    }
*/
  }
  fLoop=!fLoop;
  digitalWrite(LED_BUILTIN, fLoop);   // turn the LED on (HIGH is the voltage level)
}
