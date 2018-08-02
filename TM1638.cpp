/*
TM1638.cpp - Library implementation for TM1638.

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

#include "TM1638.h"
#include "string.h"

TM1638::TM1638(byte dataPin, byte clockPin, byte strobePin, byte numDigits, boolean activateDisplay, byte intensity)
	: TM16xx(dataPin, clockPin, strobePin, 8, numDigits, activateDisplay, intensity)
{
  _maxSegments=10;		// on the LED & KEY modules the extra segments are used to drive individual red or red/green LEDs

	clearDisplay();
	setupDisplay(activateDisplay, intensity);
}

void TM1638::setLED(byte color, byte pos)
{ // TM1638 uses 10 segments in two bytes, for the LEDs only the second byte (containing seg9-seg10) is send
    sendData((pos << 1) + 1, color);
}

void TM1638::setLEDs(word leds)
{
  for (int i = 0; i < digits; i++) {
    byte color = 0;

    if ((leds & (1 << i)) != 0) {
      color |= TM1638_COLOR_RED;
    }

    if ((leds & (1 << (i + 8))) != 0) {
      color |= TM1638_COLOR_GREEN;
    }

    setLED(color, i);
  }
}

byte TM1638::getButtons(void)
{
	// TODO: TM1638 returns 4 bytes/8 nibbles for keyscan. Each byte has K3, K2 and K1 status in lower bits of each nibble for KS1-KS2
	// Currently only the status of K3 is returned as a single byte, mixed in with K2 and K1
	// To return all a uint32_t would be more appropriate
	// 
  byte keys = 0;

  start();
  send(TM16XX_CMD_DATA_READ);
  for (int i = 0; i < 4; i++) {
    keys |= receive() << i;				// shifts the K3 bits mixed with others
  }
  stop();

  return keys;
}

void TM1638::sendChar(byte pos, byte data, boolean dot)
{ // TM1638 uses 10 segments in two bytes, for the digit displays only the first byte (containing seg1-seg8) is sent
	sendData(pos << 1, data | (dot ? 0b10000000 : 0));
}
