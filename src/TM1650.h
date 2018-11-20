/*
TM1650.h - Library for TM1637.

Copyright (C) 2011 Ricardo Batista <rjbatista at gmail dot com>
Adjusted for TM1650 by Maxint R&D, based on TM1637 code. See https://github.com/maxint-rd/

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

#ifndef TM1650_h
#define TM1650_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM16xx.h"

#define TM1650_MAX_POS 4

// TM1650 has two display modes: 8 seg x 4 grd and 7 seg x 4 grd
#define TM1650_DISPMODE_4x8 0x01
#define TM1650_DISPMODE_4x7 0x09

#define TM1650_CMD_MODE  0x48
#define TM1650_CMD_DATA_READ  0x49
#define TM1650_CMD_ADDRESS  0x68


class TM1650 : public TM16xx
{
  public:
	/** Instantiate a tm1637 module specifying the display state, the starting intensity (0-7) data and clock pins. */
  	TM1650(byte dataPin, byte clockPin, byte numDigits=4, boolean activateDisplay=true, byte intensity=7, byte displaymode = TM1650_DISPMODE_4x8);
    virtual void clearDisplay();
    virtual void setupDisplay(boolean active, byte intensity);
		virtual uint32_t getButtons();

  protected:
    virtual void bitDelay();
    virtual void start();
    virtual void stop();
    virtual void send(byte data);
    virtual void sendData(byte address, byte data);
    virtual byte TM1650::receive();
	};

#endif
