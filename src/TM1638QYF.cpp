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

#include "TM1638QYF.h"
#include "string.h"

TM1638QYF::TM1638QYF(byte dataPin, byte clockPin, byte strobePin, boolean activateDisplay, byte intensity)
	: TM16xx(dataPin, clockPin, strobePin, 8, 8, activateDisplay, intensity)
{
  _maxSegments=10;		// on the LED & KEY modules the extra segments are used to drive individual red or red/green LEDs

	clearDisplay();
}

void TM1638QYF::setDisplay(const byte values[], unsigned int length)
{
  for (int i = 0; i < digits; i++) {
	int val = 0;

	for (int j = 0; j < length; j++) {
	  val |= ((values[j] >> i) & 1) << (digits - j - 1);
	}
	
	sendData(i << 1, val);
  }
}

/*
void TM1638QYF::clearDisplay()
{
	setDisplay(NULL, 0);
}
*/

void TM1638QYF::setDisplayToString(const char* string, const word dots, const byte ignored,	const byte font[])
{
	byte values[digits];
	boolean done = false;

	memset(values, 0, digits * sizeof(byte));

	for (int i = 0; i < digits; i++) {
		if (!done && string[i] != '\0') {
		  values[i] = font[string[i] - 32] | (((dots >> (digits - i - 1)) & 1) << 7);
		} else {
		  done = true;
		  values[i] = (((dots >> (digits - i - 1)) & 1) << 7);
		}
	}

	setDisplay(values, digits);
}

void TM1638QYF::setDisplayToString(String string, const word dots, const byte ignored, const byte font[])
{
  byte values[digits];
  int stringLength = string.length();

  memset(values, 0, digits * sizeof(byte));

  for (int i = 0; i < digits; i++) {
    if (i < stringLength) {
      values[i] = font[string.charAt(i) - 32] | (((dots >> (digits - i - 1)) & 1) << 7);
    } else {
	  values[i] = (((dots >> (digits - i - 1)) & 1) << 7);
    }
  }

  setDisplay(values, digits);
}

/*
void TM1638QYF::setDisplayToBinNumber(byte number, byte dots, const byte numberFont[])
{
	byte values[digits];

	memset(values, 0, digits * sizeof(byte));

	for (int i = 0; i < digits; i++) {
		values[i] = numberFont[(number >> (digits - i - 1)) & 1] | (((dots >> (digits - i - 1)) & 1) << 7);
	}

	setDisplay(values, digits);
}

void TM1638QYF::setDisplayToHexNumber(unsigned long number, byte dots, boolean leadingZeros,
	const byte numberFont[])
{
	char values[digits + 1];

	snprintf(values, digits + 1, leadingZeros ? "%08X" : "%X", number); // ignores display size

	setDisplayToString(values, dots, 0, numberFont);
}

void TM1638QYF::setDisplayToDecNumber(unsigned long number, byte dots, boolean leadingZeros,
	const byte numberFont[])
{
	char values[digits + 1];

	snprintf(values, digits + 1, leadingZeros ? "%08ld" : "%ld", number); // ignores display size

// MMOLE: ATtiny doesn't support debug printing in library
//	Serial.println(values);
	
	setDisplayToString(values, dots, 0, numberFont);
}

void TM1638QYF::setDisplayToSignedDecNumber(signed long number, byte dots, boolean leadingZeros,
		const byte numberFont[])
{
	char values[digits + 1];

	snprintf(values, digits + 1, leadingZeros ? "%08d" : "%d", number); // ignores display size

	setDisplayToString(values, dots, 0, numberFont);
}
*/

word TM1638QYF::getButtons(void)
{
  word keys = 0;

  digitalWrite(strobePin, LOW);
  send(0x42); // B01000010 Read the key scan data
  for (int i = 0; i < 4; i++) {
	  byte rec = receive();

	  rec = (((rec & B1000000) >> 3 | (rec & B100)) >> 2) | (rec & B100000) | (rec & B10) << 3;

	  keys |= ((rec & 0x000F) << (i << 1)) | (((rec & 0x00F0) << 4) << (i << 1));
  }
  digitalWrite(strobePin, HIGH);
  
  return keys;
}

