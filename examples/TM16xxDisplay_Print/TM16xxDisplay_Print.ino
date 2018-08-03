/*
  Basic library example for TM1638. Kept small to show the simplest display functionality.
  Library based on TM1638 library by Ricardo Batista, adapted by Maxint-RD MMOLE 2018.
  
  Tested to work:
      Arduino Nano using Arduino IDE 1.8.2, Nano (Old Bootloader)), 4092 bytes flash, 135 bytes RAM

  For more information see  https://github.com/maxint-rd/TM16xx
*/

#include <TM1638.h>
#include <TM16xxDisplay.h>

TM1638 module(8, 9, 7);   // DIO=8, CLK=9, STB=7
TM16xxDisplay display(&module, 8);    // TM16xx object, 8 digits

void setup() {
  display.println(F("HELLO !"));
}

int nCount=0;
void loop() {
  delay(1000);
  display.print("Count:");
  display.println(nCount++);
}
