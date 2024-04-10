/*
InvertedTM1638.h - Library for an inverted TM1638.

Copyright (C) 2011 Ricardo Batista <rjbatista at gmail dot com>

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

#ifndef InvertedTM1638_h
#define InvertedTM1638_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM1638.h"

class InvertedTM1638 : public TM1638
{
  public:
    /** Instantiate an inverted tm1638 module specifying the display state, the starting intensity (0-7) data, clock and stobe pins. */
    InvertedTM1638(byte dataPin, byte clockPin, byte strobePin, bool activateDisplay = true, byte intensity = 7);

    /** Set the LED at pos to color (TM1638_COLOR_RED, TM1638_COLOR_GREEN or both) */
    virtual void setLED(byte color, byte pos);
    /** Returns the pressed buttons as a bit set (left to right). */
    virtual uint32_t getButtons();

  protected:
    virtual void sendChar(byte pos, byte data, bool dot);
};

#endif
