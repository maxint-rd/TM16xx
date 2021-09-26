/*
InvertedTM1638.cpp - Library implementation for inverted TM1638.

Copyright (C) 2011 Ricardo Batista (rjbatista <at> gmail <dot> com)

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

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "InvertedTM1638.h"

InvertedTM1638::InvertedTM1638(byte dataPin, byte clockPin, byte strobePin, boolean activateDisplay,
	byte intensity) : TM1638(dataPin, clockPin, strobePin, activateDisplay, intensity)
{
	// nothing to do
}

void InvertedTM1638::setLED(byte color, byte pos)
{
    sendData(((7 - pos) << 1) + 1, color);
}

uint32_t InvertedTM1638::getButtons()
{
	byte buttons = TM1638::getButtons();

	// swap each other
	buttons = (buttons & 0b01010101) <<  1 | (buttons & 0b10101010) >>  1;

	// swap each pair 
	buttons = (buttons & 0b00110011) <<  2 | (buttons & 0b11001100) >>  2;
 
	// swap each quad
	buttons = (buttons & 0b00001111) <<  4 | (buttons & 0b11110000) >>  4;

	return buttons;
}

void InvertedTM1638::sendChar(byte pos, byte data, boolean dot)
{
	TM1638::sendChar(7 - pos, (data & 0xC0) | (data & 0x07) << 3 | (data & 0x38) >> 3, dot);
}
