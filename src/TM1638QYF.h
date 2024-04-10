/*
TM1638QYF.cpp - Library implementation for QYF-TM1638 module.
The QYF-TM1638 module uses the TM1638 chip with with a 2 xcommon anode 4bit 7-segment LED display.

Made by Maxint R&D, based on TM1638 class. See https://github.com/maxint-rd/
*/

#ifndef TM1638QYF_h
#define TM1638QYF_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM16xx.h"

#define TM1638QYF_MAX_POS 8

class TM1638QYF : public TM16xx
{
  public:
    /** Instantiate a tm1638 module specifying the display state, the starting intensity (0-7) data, clock and stobe pins. */
    TM1638QYF(byte dataPin, byte clockPin, byte strobePin, bool activateDisplay = true, byte intensity = 7);

		/** Set the segments at a specific position on or off */
	  virtual void setSegments(byte segments, byte position);

		/** Clear the display */
		virtual void clearDisplay();

    /** Returns the pressed buttons as a bit set (left to right). */
    virtual uint32_t getButtons();

  private:
		byte bitmap[TM1638QYF_MAX_POS];		// store a bitmap for all 8 digit to allow common anode manipulation
		uint64_t flipDiagA8H1(uint64_t x);
		// uint64_t flipDiagA1H8(uint64_t x);
};

#endif