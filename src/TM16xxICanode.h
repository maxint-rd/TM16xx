/*
TM1618Anode - Library implementation for TM1618 with Common Anode up to 5 digits of 7 segments.
TM1618: 5x7 - 8x4 SEGxGRD, 5 x 1 single button, DIO/CLK/STB

Made by Maxint R&D, based on TM1638 class. See https://github.com/maxint-rd/
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
    /** Instantiate a TM1640 module specifying data, clock and stobe pins, the display state, the starting intensity (0-7). */
    TM16xxICanode(if_ctrl_tm16xx ctrl, byte dataPin, byte clockPin, byte strobePin, byte numDigits=4);

   /** Set the display (segments and LEDs) active or off and intensity (range from 0-7). */
   virtual void setupDisplay(bool active, byte intensity);   // For TM16xxICanode: also set the display mode (based on _maxSegments)

   	/** use alphanumeric display (yes/no) with or without segment map */	
    virtual void setAlphaNumeric(bool fAlpha=true, const byte *pMap=NULL);    // const byte aMap[]

		/** Set the segments at a specific position on or off */
	  virtual void setSegments16(uint16_t segments, byte position);
	  
		/** Clear the display */
		virtual void clearDisplay();

  private:
		//byte mapSegments(byte segments);
		uint16_t bitmap[TM16xxICanode_MAX_POS];       // store a bitmap for all 8 digits to allow common anode manipulation
		//const uint16_t *pSegmentMap=NULL;               // pointer to segment map set using setSegmentMap()
    byte _maxSegmentsX=0;
    byte _maxDisplaysX=0;
		const byte *pSegmentMapX=NULL; // pointer to segment map for alphanumeric displays. set using setAlphaNumeric()
};

#endif