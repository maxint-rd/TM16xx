/*
TM1638Anode.h - Library implementation for TM1638 with Common Anode up to 10 digits.

Made by Maxint R&D, based on TM1638 class. See https://github.com/maxint-rd/
Modified by Simon Kong Win Chang (SkullKill) https://github.com/skullkill
Schemathics of Anode connection available here https://github.com/SkullKill/ESP32-Wall-Clock-PCB and https://github.com/SkullKill/ESP32-Wall-Clock-PCB/wiki

*/

#ifndef TM1638Anode_h
#define TM1638Anode_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM16xx.h"

#define TM1638Anode_MAX_POS 10

class TM1638Anode : public TM16xx
{
  public:
    /** Instantiate a tm1638 module specifying data, clock and stobe pins, the display state, the starting intensity (0-7). */
    TM1638Anode(byte dataPin, byte clockPin, byte strobePin, bool activateDisplay = true, byte intensity = 7);

		/** Set the segments at a specific position on or off */
	  virtual void setSegments(byte segments, byte position);

		/** Clear the display */
		virtual void clearDisplay();

    /** Returns the pressed buttons as a bit set (left to right). */
    virtual uint32_t getButtons();

  private:
		byte bitmap[TM1638Anode_MAX_POS];		// store a bitmap for all 8 digit to allow common anode manipulation
		
};

#endif