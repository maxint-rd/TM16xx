/*
TM1620B.cpp - Library implementation for TM1620B.

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

#include "TM1620B.h"

TM1620B::TM1620B(byte dataPin, byte clockPin, byte strobePin, byte numDigits, boolean activateDisplay, byte intensity)
	: TM16xx(dataPin, clockPin, strobePin, TM1620B_MAX_POS, numDigits, activateDisplay, intensity)
{
	_maxSegments = 13;

	// Display mode command setting
	// Upper two grid pins are shared with the upper two segment pins, so select appropriate mode based on numDigits
	if (numDigits <= 4)
	{
		_maxSegments = 9;
		sendCommand(0x00);
	}
	else if (numDigits == 5)
	{
		_maxSegments = 8;
		sendCommand(0x01);
	}
	else if (numDigits == 6)
	{
		_maxSegments = 7;
		sendCommand(0x02);
	}
	else
	{
		_maxSegments = 6;
		sendCommand(0x03);
	}

	clearDisplay();
	setupDisplay(activateDisplay, intensity);
}

void TM1620B::bitDelay()
{
	delayMicroseconds(5);
	// When using a fast clock (such as ESP8266) a delay is needed to read bits correctly
	// NOTE: Testing on TM1620B reading keys shows that CLK should be slower than 50Khz.
	// faster than 5us delay (44 Khz) worked unreliably.
	// The datasheet specifies a maximum clock rate of 1MHz, but (unlike the TM1638) testing
	// on the ESP8266 shows this appears to be too fast.
	// for that reason the delay between reading bits should be more than 4us.
	// TODO: test medium speed MCUs like LGT8F328P (32Mhz) and STM32. Make this delay conditional on F_CPU.
}

void TM1620B::setSegments(byte segments, byte position)
{
	if (position < _maxDisplays)
	{
		sendData(position << 1, segments & 0x3F);
		sendData((position << 1) | 1, (segments & 0xC0) >> 3);
	}
}

void TM1620B::setSegments16(uint16_t segments, byte position)
{
	if (position < _maxDisplays)
	{
		sendData(position << 1, (byte)segments & 0xFF);
		sendData((position << 1) | 1, (byte)(segments >> 8) & 0x30);
	}
}

uint32_t TM1620B::getButtons(void)
{
	// Pull-up off
	pinMode(dataPin, OUTPUT);
	digitalWrite(dataPin, LOW);

	word keys_K2 = 0;
	byte received;

	start();
	send(TM16XX_CMD_DATA_READ); // send read buttons command
	for (int i = 0; i < 3; i++)
	{
		received = receive();
		keys_K2 |= ((((received & _BV(1)) >> 1) | ((received & _BV(4)) >> 3)) << (2 * i));
	}
	stop();
	return (uint32_t)keys_K2;
}
