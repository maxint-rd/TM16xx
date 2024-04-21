/*
TM1618Anode - Library implementation for TM1618 with Common Anode up to 5 digits of 7 segments.
TM1618: 5x7 - 8x4 SEGxGRD, 5 x 1 single button, DIO/CLK/STB

Made by Maxint R&D, based on TM1638 class. See https://github.com/maxint-rd/
*/

#ifndef TM1618Anode_h
#define TM1618Anode_h

#include "TM1618.h"     // TM1618Anode is derived from TM1618 (common cathode)

#define TM1618Anode_MAX_POS 5
#define TM1618Anode_MAX_SEG 7

class TM1618Anode : public TM1618
{
  public:
    /** Instantiate a TM1640 module specifying data, clock and stobe pins, the display state, the starting intensity (0-7). */
    TM1618Anode(byte dataPin, byte clockPin, byte strobePin, byte numDigits=4, bool activateDisplay = true, byte intensity = 7);

   /** Set the display (segments and LEDs) active or off and intensity (range from 0-7). */
   virtual void setupDisplay(bool active, byte intensity);   // For TM1618: also set the display mode (based on _maxSegments)

		/** Set the segments at a specific position on or off */
	  virtual void setSegments(byte segments, byte position);
	  
	  // Set mapping array to be used when displaying segments
	  // The array should contain TM1618Anode_MAX_SEG bytes specifying the desired mapping
	  virtual void setSegmentMap(const byte aMap[]);

		/** Clear the display */
		virtual void clearDisplay();

  private:
		byte mapSegments(byte segments);
		byte bitmap[TM1618Anode_MAX_POS];       // store a bitmap for all 8 digits to allow common anode manipulation
		const byte *pSegmentMap=NULL;               // pointer to segment map set using setSegmentMap()
		
};

#endif