/*
TM1620.cpp - Library implementation for TM1620.

Copyright (C) 2011 Ricardo Batista (rjbatista <at> gmail <dot> com)
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

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM1620.h"

TM1620::TM1620(byte dataPin, byte clockPin, byte strobePin, byte numDigits, boolean activateDisplay, byte intensity)
	: TM16xx(dataPin, clockPin, strobePin, TM1620_MAX_POS, numDigits, activateDisplay, intensity)
{
  _maxSegments=10;

  // Display mode command setting
  // Upper two grid pins are shared with the upper two segment pins, so select appropriate mode based on numDigits
  if (numDigits <= 4) {
    sendCommand(0x00); 

  } else if (numDigits == 5) {
    sendCommand(0x01);

  } else {
    sendCommand(0x02);
  }

	clearDisplay();
	setupDisplay(activateDisplay, intensity);
}

void TM1620::setSegments(byte segments, byte position)
{	// set 8 leds on common grd as specified
  // TM1620 uses 10 segments in two bytes
  // for the digit displays only the first byte (containing seg1-seg8) is sent
	// Only the LSB (SEG1-8) is sent to the display
	if(position<_maxDisplays)
		sendData(position << 1, segments);
}

void TM1620::setSegments16(uint16_t segments, byte position)
{	// method to send more than 8 segments (10 max for TM1620)
  // segments 1-8 are in bits 0-7 of position bytes 0,2,4,6,8,10
  // segments 13-14 are in bits 4-5 of position bytes 1,3,5,7,9,11
	if(position<_maxDisplays)
	{
		sendData(position << 1, (byte)segments&0xFF);
		sendData((position << 1) | 1, (byte)(segments>>8)&0x30);
	}
}