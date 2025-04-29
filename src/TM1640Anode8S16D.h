/*
TM1640Anode8S16D - Library implementation for TM1640 with 2 x 8-SEG x 8-digit Common Anode configuration.
Such 16 digit 7-segment CA configuration is mentioned on page 9 of the TM140B datasheet v1.2 (Chinese).
(See also issues #53 and #63 on https://github.com/maxint-rd/TM16xx).

Made by Maxint R&D, partly based on TM1640Anode class. See https://github.com/maxint-rd/
*/

#ifndef TM1640Anode8S16D_h
#define TM1640Anode8S16D_h

#include "TM1640Anode.h"   // will include Arduino.h

#define TM1640Anode8S16D_MAX_POS 16
#define TM1640Anode8S16D_MAX_SEG 8

class TM1640Anode8S16D : public TM1640
{
  public:
    /** Instantiate a TM1640 module specifying data, clock and stobe pins, the display state, the starting intensity (0-7). */
    TM1640Anode8S16D(byte dataPin, byte clockPin, byte numDigits=4, bool activateDisplay = true, byte intensity = 7);

		/** Set the segments at a specific position on or off */
	  virtual void setSegments(byte segments, byte position);
	  
	  // Set mapping array to be used when displaying segments
	  // The array should contain TM1640Anode8S16D_MAX_SEG bytes specifying the desired mapping
	  //virtual void setSegmentMap(const byte aMap[]);

		/** Clear the display */
		virtual void clearDisplay();

  private:
		//byte mapSegments(byte segments);
		byte bitmap[TM1640Anode8S16D_MAX_POS];       // store a bitmap for all 16 digits to allow common anode manipulation
		//const byte *pSegmentMap=NULL;               // pointer to segment map set using setSegmentMap()
};

#endif // #TM1640Anode8S16D_h