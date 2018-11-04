/*
  TM16xx setSegments example to test all segments of a TM16xx display.

  This example is for testing the basic setSegments function of the TM16xx library.
  That function should work for each supported TM16xx chip, but the number of segments
  supported is different per chip:
     Type    segments x digits     buttons       interface
     TM1637  8 x 6 (common anode)  8 x 2 single  DIO/CLK
     TM1638  10 x 8                8 x 3 multi   DIO/CLK/STB
     TM1640  8 x 16                n/a           DIN/CLK
     TM1650  8 x 4                 7x4           DIO/CLK (SDA/SCK?)
     TM1668  10 x 7 - 13 x 4       10 x 2 multi  DIO/CLK/STB

  It was tested on Arduino Nano (old bootloader) in Arduino IDE 1.8.2. (TM1638: 4698B flash, 292B RAM)

  Made by Maxint-RD MMOLE 2018. see GitHub.com/maxint-rd/TM16xx
*/

//#include <TM1637.h>
//TM1637 module(3, 2);    //  DIO=3, CLK=2
//TM1637 module(5, 4);  // DIO=5, CLK=4

#include <TM1638.h>
TM1638 module(8, 9, 7);   // DIO=8, CLK=9, STB=7

//#include <TM1638QYF.h>
//TM1638QYF module(8, 9, 7);   // DIO=8, CLK=9, STB=7

//#include <TM1668.h>
//TM1668 module(8, 9, 10);   // DIO=8, CLK=9, STB=7
//TM1668 module(3, 2, 1, 7, true, 7);    //  DIO=3, CLK=2, STB=1, byte numDigits = 4, boolean activateDisplay = true, byte intensity = 7, displaymode=TM1668_DISPMODE_7x10

// function to show some indicative blinks
void Blink(int nDelay=500, int nTimes=1)
{
  for(int i=0; i<nTimes; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);    // switch on (or active low LED off for ESP)
    delay(nDelay);
    digitalWrite(LED_BUILTIN, LOW);    // switch off (or active low LED on for ESP)
    delay(nDelay);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println(F("TM16xx_setSegments example"));
  Serial.println(F("TM1638: DIO=8, CLK=9, STB=7"));

  module.setupDisplay(true, 7);     // set display on at highest intensity

  Serial.println(F("Clear"));
  module.clearDisplay();

  Serial.println(F("Display string"));
  Blink(10);
  module.setDisplayToString("HALO");
  delay(1000);  

  Blink(10);
  module.clearDisplay();
  module.setDisplayToString("HALO1234", 0xF0);
  delay(1000);  

  Serial.println(F("Setup done..."));
}

void loop()
{
  Blink(10);
  module.clearDisplay();
  Blink(10);

  for(byte nPos=0; nPos<8; nPos++)
  {
    Serial.print(F("Loop pos: "));
    Serial.println(nPos);
    //for(uint16_t nVal=0; nVal<=0x3FF; nVal=nVal*2+1)
    for(uint16_t nVal=0; nVal<=0xFF; nVal=nVal*2+1)     // max 8 segments on TM1638QYF module
    {
      module.setSegments(nVal, nPos);   // note: using int type confuses compiler when module class supports > 8 segments (TM1638, TM1668)
      delay(100);
    }
    Blink(10);
  }
  delay(1000);
}
