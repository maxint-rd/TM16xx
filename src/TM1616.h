/*
TM1616.h - Library for TM1616. DIN/CLK/STB, 7x4 LED, no buttons.
Made by Maxint R&D. See https://github.com/maxint-rd/TM16xx
*/

#ifndef TM1616_h
#define TM1616_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM16xx.h"

#define TM1616_MAX_POS 4


class TM1616 : public TM16xx
{
  public:
    /** Instantiate a TM1616 module specifying data, clock and strobe pins, the number of digits, the display state, the starting intensity (0-7). */
    TM1616(byte dataPin, byte clockPin, byte strobePin, byte numDigits=TM1616_MAX_POS, bool activateDisplay = true, byte intensity = 7);

		/** Override standard behaviour where this chip acts differently */
	  virtual void setSegments(byte segments, byte position);
		virtual void clearDisplay();
};

#endif