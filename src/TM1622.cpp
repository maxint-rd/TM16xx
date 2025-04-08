/*
TM1622.cpp - Library implementation for TM1622 based on TM1621 + TM16xxIC.

Driver class for TM1622 based on the TM1621 class and the generic TM16xxIC class.
It offers simple instantiation, similar to regular TM16xx classes.

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment displays and use Adafruit GFX on matrix displays.
*/

#include "TM1622.h"

void TM1622::setSegments16(uint16_t segments, byte position)
{
  // Map segments if specified for alternative segment wiring.
  if(pSegmentMap)
    segments=mapSegments16(segments, pSegmentMap);
  
  // assume 8-COM, 1-SEG layout by default: TM1622_LAYOUT_8C1S_PGFE_DCBA
  sendData((position<<1), segments & 0x0F);     // SEGx+1: segments ABCD on first address (S0/COM0-COM3)
  sendData(((position<<1)|1), (segments&0xF0)>>4);      // SEGx+0: segments EFGP on second address (S0/COM4-COM7)
}

void TM1622_DM8BA10::setSegments16(uint16_t segments, byte position)
{
  // Map segments if specified for alternative segment wiring.
  if(pSegmentMap)
    segments=mapSegments16(segments, pSegmentMap);

  // conversion of regular TM16xx segments/position to TM1622 LCD segments/common
  if(this->_layout==TM1622_LAYOUT_DM8BA10)
  { // The DM8BA10 LCD module has 10 16-segment digits plus 9 dots. The dots are on 3 separate addresses
    // To allow using the regular 14-segment font, segments A1+A2 and D1+D2 are duplicated, other segments use SEGMAP_14SEG_DM8BA10
    position=this->digits-position-1;     // reverse the position order
    
    // handle separate addresses for dots.
    bool fDot=(segments & bit(7))?true:false; // remember if a dot is set (mapped to bit 7)
    _uDots=_uDots & ~bit(position-1);   //  there is no dot on position 0 (rightmost digit)
    if(fDot) _uDots|=bit(position-1);
    segments = segments & ~bit(7); // reset dot on bit 7
    
    // duplicate A1/A2 and D1/D2 segments for use of 14-segment font
    if(segments & bit(15)) segments|=bit(7);     // duplicate A to A1/A2
    if(segments & bit(8)) segments|=bit(0);    // duplicate D to D1/D2

    // send segment data to the four 4-bit addresses (assume segment order is already changed properly by mapping)
    sendData((position<<2)|3, (segments&0x0F00)>>8);      // SEGx+0: segments on fourth address D1/C/L/G2
    sendData(((position<<2)|2), (segments & 0xF000)>>12);     // SEGx+1: segments on third address K/B/J/A2
    sendData(((position<<2)), (segments & 0x00F0)>>4);     // SEGx+1: segments on first address G1/F/H/A1
    sendData(((position<<2)|1), segments & 0x000F);     // SEGx+1: segments on second address D2/E/M/N

    // show any dots specific to the current position
    // dots 7,8,9 are on address 41; dots 4,5,6 are on address 43; dots 1,2,3 are on address 45 (no dot 0)
    if(position<=3) sendData(45, (_uDots&0x0007));
    else if (position<=6) sendData(43, (_uDots&0x0038)>>3);
    else if (position<=9) sendData(41, (_uDots&0x01C0)>>6);
  }
  else
  { // assume 8-COM, 1-SEG layout by default: TM1622_LAYOUT_8C1S_PGFE_DCBA
    sendData((position<<1), segments & 0x0F);     // SEGx+1: segments ABCD on first address (S0/COM0-COM3)
    sendData(((position<<1)|1), (segments&0xF0)>>4);      // SEGx+0: segments EFGP on second address (S0/COM4-COM7)
  }
}

