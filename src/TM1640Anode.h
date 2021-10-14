/*
TM1640Anode.h - Library implementation for TM1640 with Common Anode, up to 8 digits of 16-segments.

Made by Maxint R&D, based on TM1638 class. See https://github.com/maxint-rd/

*/

#ifndef TM1640Anode_h
#define TM1640Anode_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM1640.h"

#define TM1640Anode_MAX_POS 8
#define TM1640Anode_MAX_SEG 16

class TM1640Anode : public TM1640
{
  public:
    /** Instantiate a TM1640 module specifying data, clock and stobe pins, the display state, the starting intensity (0-7). */
    TM1640Anode(byte dataPin, byte clockPin, byte numDigits=4, boolean activateDisplay = true, byte intensity = 7);

    /** Set an Ascii character on a specific location (overloaded for 15-segment display) */
		virtual void sendAsciiChar(byte pos, char c, boolean dot); // public method to allow calling from TM16xxDisplay

		/** Set the segments at a specific position on or off */
	  virtual void setSegments(byte segments, byte position);   // will duplicate G to G1/G2 in 15-segment
	  virtual void setSegments16(uint16_t segments, byte position);
	  
	  // Set mapping array to be used when displaying segments
	  // The array should contain TM1640Anode_MAX_SEG bytes specifying the desired mapping
	  virtual void setSegmentMap(const byte aMap[]);

		/** Clear the display */
		virtual void clearDisplay();

  private:
		uint16_t mapSegments16(uint16_t segments);
		uint16_t bitmap[TM1640Anode_MAX_POS];       // store a bitmap for all 8 digits to allow common anode manipulation
		const byte *pSegmentMap=NULL;               // pointer to segment map set using setSegmentMap()
		
};

#endif