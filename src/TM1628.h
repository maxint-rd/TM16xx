/*
TM1628.h - Library for TM1628.

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment displays and use Adafruit GFX on matrix displays.

Made by Maxint R&D. See https://github.com/maxint-rd
*/

#ifndef TM1628_h
#define TM1628_h

#include "TM16xx.h"   // will include Arduino.h

#define TM1628_MAX_POS 7


class TM1628 : public TM16xx
{
  public:
    /** Instantiate a TM1628 module specifying data, clock and stobe pins, the number of digits, the display state, the starting intensity (0-7). */
    TM1628(byte dataPin, byte clockPin, byte strobePin, byte numDigits=TM1628_MAX_POS, bool activateDisplay = true, byte intensity = 7);

    /** Set the segments at a specific position on or off */
    virtual void setSegments(byte segments, byte position);
    virtual void setSegments16(uint16_t segments, byte position);

    virtual void clearDisplay();

    /** Set an Ascii character on a specific location (overloaded for 15-segment display) */
    virtual void sendAsciiChar(byte pos, char c, bool dot, const byte font[] = TM16XX_FONT_DEFAULT); // public method to allow calling from TM16xxDisplay

    // Set mapping array to be used when displaying segments
    // The array should contain _maxSegments bytes specifying the desired mapping
//    virtual void setSegmentMap(const byte aMap[]);  // OK on ESP32 core 3.0.2, tinyPico, not on ESP8266
    virtual void setSegmentMap(PGM_P aMap);  // OK???? on ESP32 core 3.0.2, tinyPico, works for  ESP8266

    /** Returns the pressed buttons as a bit set (left to right). */
    virtual uint32_t getButtons();

  protected:
    virtual void bitDelay();

  private:
    uint16_t mapSegments16(uint16_t segments);
//    const PROGMEM byte *_pSegmentMap=NULL;               // pointer to segment map set using setSegmentMap()
    PGM_P _pSegmentMap=NULL;               // pointer to segment map set using setSegmentMap() - PGM_P needed in ESP8266 core 3.0.2

};

// Regular segment order: 
//    0123456789012345
//    ABCDEFGPgHJKLMNX (G=G1, g=G2, P=point, X=not used)
// For alternative segment wiring, use setSegmentMap(). Example:   module.setSegmentMap(TM1628_SEGMAP_2X5241AS);
// pinout 1-19 for 19-pins dual 5241AS LED-display module: EN1MKGA2B4FH3JLgDPC (G=G1, g=G2, P=point)
// TM1628 supports 13x4 mode, requiring G1=G2, no DP.
//const PROGMEM byte TM1628_SEGMAP_BB2X5241AS[]={3, 2, 12, 10, 6, 1, 9, 15 /*11*/, 9, 0, 7, 4, 8, 5, 11, 15};      // mapping for LED-display dual 5241AS combined 19-pins, having G1=G2,no DP to allow for max 13 segments in breadboard
//const PROGMEM byte TM1628_SEGMAP_2X5241AS[]={0, 1, 2, 3, 4, 5, 6, 15, 6, 7, 8, 9, 10, 11, 12, 15};      // mapping for LED-display dual 5241AS combined 19-pins, having G1=G2,no DP to allow for max 13 segments
const byte TM1628_SEGMAP_BB2X5241AS[] PROGMEM = {3, 2, 12, 10, 6, 1, 9, 15 /*11*/, 9, 0, 7, 4, 8, 5, 11, 15};      // mapping for LED-display dual 5241AS combined 19-pins, having G1=G2,no DP to allow for max 13 segments in breadboard
const byte TM1628_SEGMAP_2X5241AS[] PROGMEM = {0, 1, 2, 3, 4, 5, 6, 15, 6, 7, 8, 9, 10, 11, 12, 15};      // mapping for LED-display dual 5241AS combined 19-pins, having G1=G2,no DP to allow for max 13 segments
//{0, 1, 2, 3, 4, 5, 6, 14, 7, 8, 9, 10, 11, 12, 13, 15};      // mapping for LED-display dual 5241AS combined 19-pins, having G1=G2,no DP.


#endif