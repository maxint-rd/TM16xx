/*
TM1628.h - Library for TM1628.

Copyright (C) 2011 Ricardo Batista <rjbatista at gmail dot com>
Rewrite for TM16xx library by Maxint R&D. See https://github.com/maxint-rd/

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

#ifndef TM1628_h
#define TM1628_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM16xx.h"

#define TM1628_MAX_POS 7


class TM1628 : public TM16xx
{
  public:
    /** Instantiate a TM1628 module specifying data, clock and stobe pins, the number of digits, the display state, the starting intensity (0-7). */
    TM1628(byte dataPin, byte clockPin, byte strobePin, byte numDigits=TM1628_MAX_POS, boolean activateDisplay = true, byte intensity = 7);

		/** Set the segments at a specific position on or off */
	  virtual void setSegments(byte segments, byte position);
	  virtual void setSegments16(uint16_t segments, byte position);

    /** Returns the pressed buttons as a bit set (left to right). */
    virtual uint32_t getButtons();

  protected:
    virtual void bitDelay();

};

#endif