/*
TM1637.h - Library for TM1637.

Copyright (C) 2011 Ricardo Batista <rjbatista at gmail dot com>
Adjusted for TM1637 by Maxint R&D, based on TM1640 code. See https://github.com/maxint-rd/

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TM1637_h
#define TM1637_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM16xx.h"

#define TM1637_MAX_POS 6

class TM1637 : public TM16xx
{
  public:
	/** Instantiate a tm1637 module specifying the display state, the starting intensity (0-7) data and clock pins. */
  	TM1637(byte dataPin, byte clockPin, byte numDigits=4, boolean activateDisplay=true, byte intensity=7);
		virtual uint32_t getButtons();

  protected:
    virtual void bitDelay();
    virtual void stop();
    virtual void send(byte data);
    uint16_t _uLeds;		// rooom to store status of LEDS that can be attached to GRD 5 and 6
};

#endif
