/*
TM1630.cpp - Library implementation for TM1630.

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

#include "TM1630.h"

TM1630::TM1630(byte dataPin, byte clockPin, byte strobePin, byte numDigits, bool activateDisplay, byte intensity)
  : TM16xx(dataPin, clockPin, strobePin, TM1630_MAX_POS, numDigits, activateDisplay, intensity)
{
  // Display mode command setting
  // Upper two grid pins are shared with the upper two segment pins, so select appropriate mode based on numDigits
  if (numDigits <= 4) {
    _maxSegments = 8;
    sendCommand(0x00);
  } else {
    _maxSegments = 7;
    sendCommand(0x01);
  }

  clearDisplay();
  setupDisplay(activateDisplay, intensity);
}

// TM1630 does not have SEG1 pin, but instead has SEG14.
void TM1630::setSegments(byte segments, byte position)
{ // set 8 leds on common grd as specified
  // TM1630 uses 8 segments in two bytes
  // for the digit displays only the first byte (containing seg2-seg8) is sent
  // Only the LSB (SEG2-8) is sent to the display
  if (position < _maxDisplays)
    sendData(position << 1, (byte)(segments << 1) & 0xFE);
}

void TM1630::setSegments16(uint16_t segments, byte position)
{ // method to send more than 8 segments (9 max for TM1630)
  // segments 2-8 are in bits 1-7 of position bytes 0,2,4,6,8,10,12
  // segment 14 is in bit 5 of position bytes 1,3,5,7,9,11,13
  if (position < _maxDisplays)
  {
    sendData(position << 1, (byte)(segments << 1) & 0xFE);
    sendData((position << 1) | 1, (byte)(segments << 2) & 0x20);
  }
}

uint32_t TM1630::getButtons(void)
{ // Keyscan data on the TM1630 is 1x7 keys, received as an array of 4 bytes.
  // Of each byte the bits B1/B4 represent status of the connection of K2 to KS2-KS8
  // The return value is a 32 bit value containing button scans for K2, the high word is for K2 and the low word is empty.
  word keys_K2 = 0;
  byte received;

  start();
  send(TM16XX_CMD_DATA_READ);		// send read buttons command
  for (int i = 0; i < 4; i++) {
    received = receive();
    keys_K2 |= ((((received & _BV(1)) >> 1) | ((received & _BV(4)) >> 3)) << (2 * i));			// bit 1 for K2/KS3|5|7 and bit 4 for K2/KS2|4|6|8
  }
  stop();
  return ((uint32_t)keys_K2);
}
