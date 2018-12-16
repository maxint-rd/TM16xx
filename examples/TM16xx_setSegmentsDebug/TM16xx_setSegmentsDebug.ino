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

  This is the debugging version of the basic TM16xx_setSegments example.
  The Blink function and debug print functions were added to allow easy debugging using a logical analyser.
  To compile on smaller processors (such as ATtiny44) you can safe space by disabling serial printing: set #define OPT_DEBUG_PRINT 0

  It was compiled for Arduino Nano (old bootloader) in Arduino IDE 1.8.2. (TM1638: 4944B flash, 292B RAM, TM1650: 5434B flash, 286B RAM)

  It was compiled for ATTiny44A (ATtinyCore/LTO enabled) in Arduino IDE 1.8.2.
     - TM1637: 4012 flash/155B RAM => 2954/101 (without serial)
     - TM1638: 4312 flash/159B RAM => 3252/105 (without serial)
     - TM1638QYF: 4648 flash/161B RAM => 3588/107 (without serial)
     - TM1640: 3844 flash/153B RAM => 2784/99 (without serial)
     - TM1650: 4544 flash/153B RAM (with serial) => 3486/99 (without serial)
     - TM1668: 4160 flash/161B RAM (with serial) => 3102/107 (without serial)

  Made by Maxint-RD MMOLE 2018. see GitHub.com/maxint-rd/TM16xx
*/

// Pin suggestions:
// ESP8266 (Wemos D1): data pin 5 (D1), clock pin 4 (D2)
// ATtiny85: data pin 3, clock pin 2 (LED_BUILTIN: 0)
// ATtiny44A: data pin 9, clock pin 10 (LED_BUILTIN: 8)


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


#define OPT_DEBUG_PRINT 0
//#include <TinyDebugSerial.h>        // NOTE: TinyDebugSerial is not compatible with ATtinyCcore
#if(OPT_DEBUG_PRINT)
#define DEBUG_BEGIN(x) Serial.begin(x)
#define DEBUG_PRINT(x) (Serial.print(x))
#define DEBUG_PRINTLN(x) (Serial.println(x))
#define DEBUG_PRINTLNHEX(x) (Serial.println(x, HEX))
//#define LED_BUILTIN 1       // on ATtinyCore ATtiny85: Serial uses TX=0, RX=1, RX can be reused as output pin
#else
#define DEBUG_BEGIN(x) 
#define DEBUG_PRINT(x) 
#define DEBUG_PRINTLN(x) 
#define DEBUG_PRINTLNHEX(x) 
#endif


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
  DEBUG_BEGIN(9600);
  DEBUG_PRINTLN(F("TM16xx_setSegments example"));
  DEBUG_PRINTLN(F("TM1638: DIO=8, CLK=9, STB=7"));
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  module.setupDisplay(true, 1);     // set display on at low intensity

  DEBUG_PRINTLN(F("Clear"));
  module.clearDisplay();

  DEBUG_PRINTLN(F("String"));
  Blink(10);
  module.setDisplayToString("HALO");
  delay(500);  

  module.clearDisplay();
  module.setupDisplay(true, 7);     // set display on at highest intensity
  DEBUG_PRINTLN(F("Setup done..."));
}

void loop()
{
  Blink(10);
  module.clearDisplay();
  Blink(10);

  for(byte nPos=0; nPos<16; nPos++)       // chip specific number of segments, 16 positions (columns) on TM1640, most others have less
  {
    DEBUG_PRINT(F("Loop pos: "));
    DEBUG_PRINTLN(nPos);
    //for(uint16_t nVal=0; nVal<=0x3FF; nVal=nVal*2+1)
    for(int nVal=1; nVal<=0xFF; nVal=nVal*2)     // max 8 segments on TM1638QYF module.
    {
      module.setSegments(nVal, nPos);   // note: you can use setSegments16() when module class supports > 8 segments (TM1638, TM1668)
      delay(100);
    }
    Blink(10);
  }
  delay(100);
}
