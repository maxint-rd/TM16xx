/*
TM1623.h - Library implementation for TM1623 based on generic TM16xxIC.

Alternative class for TM1623 based on the generic TM16xxIC class.
It offers simple instantiation, similar to regular TM16xx classes.
This is a header file with only constructors. All functionality is implemented in parent classes TM16xxIC and TM16xx.
The overhead is up to 126 bytes flash, tested on CH32V003, optimized for size -Os without LTO. With LTO the overhead was only 8 bytes.

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment displays and use Adafruit GFX on matrix displays.
*/

#ifndef TM1623_h
#define TM1623_h

#include "TM16xxIC.h"   // will include TM16xx.h

#define TM1623_MAX_POS 7  // TM1624: 14x4 - 11x7

class TM1623 : public TM16xxIC
{
  public:
    // NOTE: to ensure calling the overloaded constructor and not the TM16xx base constructor, we need to match parameters and cannot use defaults
  	TM1623(byte dataPin, byte clockPin, byte strobePin, byte numDigits): TM16xxIC(IC_TM1623, dataPin, clockPin, strobePin, numDigits) {}
  	TM1623(byte dataPin, byte clockPin, byte strobePin): TM1623(dataPin, clockPin, strobePin, 4) {}
    // NOTE: to use DIO on TM1623, DIN must be shorted to DOUT. See circuit (figure 15) in datasheet v2.4 on page 13.
};

#endif // #ifndef TM1623_h
