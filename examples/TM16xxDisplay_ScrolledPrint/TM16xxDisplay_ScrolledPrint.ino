/*
  Scrolling printed text library example for TM1638.
  Library based on TM1638 library by Ricardo Batista, adapted by Maxint-RD MMOLE 2018.
  
  Tested to work:
      Arduino Nano using Arduino IDE 1.8.2, Nano (Old Bootloader)), 5269 bytes flash, 342 bytes RAM

  For more information see  https://github.com/maxint-rd/TM16xx
*/

#include <TM1638.h>
#include <TM16xxDisplay.h>

TM1638 module(8, 9, 7);   // DIO=8, CLK=9, STB=7
#define NUM_DIGITS 8
TM16xxDisplay display(&module, NUM_DIGITS);    // TM16xx object, 8 digits

void setup() {
  Serial.begin(115200);
  Serial.println(F("TM1638 scrolled print example"));
  Serial.println(F("TM1638: DIO=8, CLK=9, STB=7"));
  display.println(F("HELLO !"));
  delay(1000);
}

int nPos=0;
char szText[]="Hallo, dit is een test. . . 123";
void loop() {
  Serial.print("Pos:");
  Serial.println(nPos);
  display.setCursor(nPos);
  display.println(szText);
  nPos--;
  if(nPos == 0-strlen(szText)-1)
  {
    display.clear();
    nPos=NUM_DIGITS;
  }
  delay(200);
}
