/*
TM1638.h - Library for TM1638.

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

#ifndef TM1638_h
#define TM1638_h

#include "TM16xx.h"

#define TM1638_MAX_POS 8

#define TM1638_COLOR_NONE   0
#define TM1638_COLOR_RED    1
#define TM1638_COLOR_GREEN  2

class TM1638 : public TM16xx
{
  public:
    /** Instantiate a TM1638 module specifying data, clock and stobe pins, the number of digits
        DEPRECATED: activation and intensity are no longer used by constructor. Use begin() or setupDisplay() instead. */
    TM1638(byte dataPin, byte clockPin, byte strobePin, byte numDigits=8, bool activateDisplay = true, byte intensity = 7);

    /** Set the up to 8 segments at a specific position on or off */
    virtual void setSegments(byte segments, byte position);

    /** Set the up to 16 segments at a specific position on or off  (max. 10 for TM1638) */
    virtual void setSegments16(uint16_t segments, byte position);

    /** Set the LED at pos to color (TM1638_COLOR_RED, TM1638_COLOR_GREEN or both) */
    virtual void setLED(byte color, byte pos);

    /** Set the LEDs. MSB byte for the green LEDs, LSB for the red LEDs */
    virtual void setLEDs(word led);

    /** Returns the pressed buttons as a bit set (left to right). */
    virtual uint32_t getButtons();
};

#endif