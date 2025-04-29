/*
TM1640.h - Library for TM1640.

Copyright (C) 2011 Ricardo Batista <rjbatista at gmail dot com>
Adjusted for TM1640 by Maxint R&D, based on orignal code. See https://github.com/maxint-rd/

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

#ifndef TM1640_h
#define TM1640_h

#include "TM16xx.h"

#define TM1640_MAX_POS 16

class TM1640 : public TM16xx
{
  public:
		/** Instantiate a TM1640 module specifying data and clock pins, number of digits.
        DEPRECATED: activation and intensity are no longer used by constructor. Use begin() or setupDisplay() instead. */
  	TM1640(byte dataPin, byte clockPin, byte numDigits=16, bool activateDisplay = true, byte intensity = 7);

  protected:
    //virtual void bitDelay();
    virtual void start();
    virtual void stop();
    virtual void send(byte data);
};

#endif
