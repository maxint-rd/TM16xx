/*
  Basic library example for TM1640. Kept small to fit in the limited space of an ATtiny44.

  Library based on TM1638 library by Ricardo Batista, adapted by Maxint-RD MMOLE 2018.
  Confirmed to work in the following environments:
      ATtiny44A using Arduino IDE 1.8.2 and ATTinyCore (8MHz, LTO enabled), 3232 bytes flash, 103 bytes RAM
      ATtiny44A using Arduino IDE 1.8.2 and ATtiny Microcontrolers (8MHz), 3212 bytes flash, 103 bytes RAM ==> 2892/95 ==> 2878/95

  Only compiled: not tested yet:
      Arduino Nano using Arduino IDE 1.8.2, Nano (Old Bootloader)), 3176 bytes flash, 95 bytes RAM

  For more information see  https://github.com/maxint-rd/TM16xx
*/
#include <TM1640.h>

#if !defined(LED_BUILTIN)
#define LED_BUILTIN 4
#endif

// Define a 4-digit display module. Pin suggestions:
// ESP8266 (Wemos D1): data pin 5 (D1), clock pin 4 (D2)
// ATtiny44A: data pin 9, clock pin 10 (LED_BUILTIN: 8 on ATTinyCore)
TM1640 module(9, 10, 4);    // data, clock, 4 digits

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  module.setDisplayToString("HALO");
  delay(500);                        // wait
  //module.clearDisplay();
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  module.setupDisplay(true, 7);   // sometimes noise on the line may change the intensity level
  int nTime = ((millis() / 1000) / 60) * 100 + (millis() / 1000) % 60; // minutes+seconds as integer
  module.setDisplayToDecNumber(nTime, _BV(4)); // display dot on digit 4
  delay(500);                        // wait
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  module.setDisplayToDecNumber(nTime, _BV(3)); // display colon on digit 3
  delay(500);                        // wait
}
