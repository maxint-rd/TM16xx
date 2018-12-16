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
  For setting more than 8 segments the similar function setSegments16 can be used.

  It was tested on Arduino Nano (old bootloader) in Arduino IDE 1.8.2. (TM1638: 4944B flash, 292B RAM, TM1650: 5434B flash, 286B RAM)
  It was tested on ATTiny44A (ATtinyCore/LTO enabled) in Arduino IDE 1.8.2. (TM1640: 2738B flash, 107B RAM)
  
  Made by Maxint-RD MMOLE 2018. see GitHub.com/maxint-rd/TM16xx
*/

//#include <TM1637.h>
//TM1637 module(3, 2);    //  DIO=3, CLK=2
//TM1637 module(5, 4);  // DIO=5, CLK=4

//#include <TM1638.h>
//TM1638 module(8, 9, 7);   // DIO=8, CLK=9, STB=7

//#include <TM1638QYF.h>
//TM1638QYF module(8, 9, 7);   // DIO=8, CLK=9, STB=7

#include <TM1640.h>
TM1640 module(9, 10);  // DA=9, CLK=10

//#include <TM1650.h>
//TM1650 module(A4, A5);  // SDA=A4, SCL=A5

//#include <TM1668.h>
//TM1668 module(8, 9, 10);   // DIO=8, CLK=9, STB=7
//TM1668 module(3, 2, 1, 7, true, 7);    //  DIO=3, CLK=2, STB=1, byte numDigits = 7, boolean activateDisplay = true, byte intensity = 7, displaymode=TM1668_DISPMODE_7x10

void setup()
{
  module.setDisplayToString("HALO");
  delay(1000);  

  module.clearDisplay();
  module.setDisplayToString("HALO1234", 0xF0);
  delay(1000);  
}

void loop()
{
  module.clearDisplay();
  for(byte nPos=0; nPos<16; nPos++)     // 16 positions (columns) on TM1640, most others have less
  {
    for(int nVal=0; nVal<=0xFF; nVal=nVal*2+1)     // max 8 segments on TM1638QYF module.
    {
      module.setSegments(nVal, nPos);   // note: you can use setSegments16() when module class supports > 8 segments (TM1638, TM1668)
      delay(100);
    }
  }
  delay(1000);
}
