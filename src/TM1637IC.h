/*
TM1637IC.cpp - Library implementation for TM1637 based on generic TM16xxIC.

Alternative class for TM1637 based on the generic TM16xxIC class.
It offers simple instantiation, similar to regular TM16xx classes.
This is a header file with only constructors. All functionality is implemented in parent classes TM16xxIC and TM16xx.
The overhead is 126 bytes flash, tested on CH32V003, optimized for size -Os without LTO. With LTO the overhead was only 8 bytes.

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment displays and use Adafruit GFX on matrix displays.
*/

#ifndef TM1637IC_h
#define TM1637IC_h

#include "TM16xxIC.h"   // will include TM16xx.h

#define TM1637_MAX_POS 6

class TM1637IC : public TM16xxIC
{
  public:
  	TM1637IC(byte dataPin, byte clockPin, byte numDigits=4);
};

TM1637IC::TM1637IC(byte dataP, byte clockP, byte numDigits) : TM16xxIC(IC_TM1637, dataP, clockP, dataP, numDigits) {}


#endif // #ifndef TM1637IC_h
