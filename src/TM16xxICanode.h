/*
TM16xxICanode - Library implementation for TM16xxIC used in Common Anode configuration.

Tested configurations:
  TM1640: 16 GRD-segments x 8 SEG-digits 

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment displays and use Adafruit GFX on matrix displays.

Made by Maxint R&D. See https://github.com/maxint-rd
*/

#ifndef TM16xxICanode_h
#define TM16xxICanode_h

#include "TM16xxIC.h"     // TM16xxICanode is derived from TM16xxIC (common cathode)

// Common Anode segment maps
const byte SEGMAP_15SEG_BB2X5241BS[] PROGMEM = {12, 11, 8, 6, 1, 9, 13, 7, 5, 10, 15, 14, 4, 3, 2, 0};      // mapping for LED-display dual 5241BS CA connecting 15 segments to TM1640 GRD2-GRD16 on breadboard


#define TM16xxICanode_MAX_POS 16
#define TM16xxICanode_MAX_SEG 16

class TM16xxICanode : public TM16xxIC
{
  public:
    /** Instantiate a module; specifying data, clock and strobe pins and the number of digits (default is 4 digits) */
    TM16xxICanode(if_ctrl_tm16xx ctrl, byte dataPin, byte clockPin, byte strobePin, byte numDigits=4);

    /** Constructor for chips with only data and clock */
    TM16xxICanode(if_ctrl_tm16xx ctrl, byte dataP, byte clockP) : TM16xxICanode(ctrl, dataP, clockP, dataP, 4) {};

    /** Set the display (segments and LEDs) active or off and intensity (range from 0-7). */
    virtual void setupDisplay(bool active, byte intensity);   // TODO for TM16xxICanode: also set the display mode (based on _maxSegments)

    /** use alphanumeric display (yes/no) with or without segment map */  
    virtual void setAlphaNumeric(bool fAlpha=true, const byte *pMap=NULL);    // const byte aMap[]

    /** Set the segments at a specific position on or off */
    virtual void setSegments16(uint16_t segments, byte position);
    
    /** Clear the display */
    virtual void clearDisplay();

  private:
    uint16_t bitmap[TM16xxICanode_MAX_POS];       // store a bitmap for all 8 digits to allow common anode manipulation
    byte _maxSegmentsX=0;
    byte _maxDisplaysX=0;
    const byte *pSegmentMapX=NULL; // pointer to segment map for alphanumeric displays. set using setAlphaNumeric()
};

#endif