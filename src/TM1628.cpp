/*
TM1628.cpp - Library implementation for TM1628.

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

#include "TM1628.h"

TM1628::TM1628(byte dataPin, byte clockPin, byte strobePin, byte numDigits, boolean activateDisplay, byte intensity)
	: TM16xx(dataPin, clockPin, strobePin, TM1628_MAX_POS, numDigits, activateDisplay, intensity)
{
  _maxSegments=13;

  // Display mode command setting
  // Upper two grid pins are shared with the upper two segment pins, so select appropriate mode based on numDigits
  if (numDigits <= 4) {
    sendCommand(0x00); 
  } else if (numDigits == 5) {
	  _maxSegments=12;
    sendCommand(0x01);
  } else if (numDigits == 6) {
	  _maxSegments=11;
    sendCommand(0x02);
  } else {
	  _maxSegments=10;
    sendCommand(0x03);
  }

	clearDisplay();
	setupDisplay(activateDisplay, intensity);
}

void TM1628::bitDelay()
{
	delayMicroseconds(5);
	// When using a fast clock (such as ESP8266) a delay is needed to read bits correctly
	// NOTE: Testing on TM1628 reading keys shows that CLK should be slower than 50Khz.
	// faster than 5us delay (44 Khz) worked unreliably.
	// The datasheet specifies a maximum clock rate of 1MHz, but (unlike the TM1638) testing 
	// on the ESP8266 shows this appears to be too fast.
	// for that reason the delay between reading bits should be more than 4us.
	// TODO: test medium speed MCUs like LGT8F328P (32Mhz) and STM32. Make this delay conditional on F_CPU.
}


void TM1628::setSegments(byte segments, byte position)
{	// set 8 leds on common grd as specified
  // TM1628 uses 10 segments in two bytes
  // for the digit displays only the first byte (containing seg1-seg8) is sent
	// Only the LSB (SEG1-8) is sent to the display
	if(position<_maxDisplays)
		sendData(position << 1, segments);
}

void TM1628::setSegments16(uint16_t segments, byte position)
{	// method to send more than 8 segments (13 max for TM1628)
  // segments 1-8 are in bits 0-7 of position bytes 0,2,4,6,8,10,12
  // segments 9-13 are in bits 0-1 and 3-5 of position bytes 1,3,5,7,9,11,13
	if(position<_maxDisplays)
	{
		sendData(position << 1, (byte)segments&0xFF);
		sendData((position << 1) | 1, (byte)(segments>>8)&0x30);
	}
}

uint32_t TM1628::getButtons(void)
{	// Keyscan data on the TM1628 is 2x10 keys, received as an array of 5 bytes (same as TM1668).
	// Of each byte the bits B0/B3 and B1/B4 represent status of the connection of K1 and K2 to KS1-KS10
	// Byte1[0-1]: KS1xK1, KS1xK2
	// The return value is a 32 bit value containing button scans for both K1 and K2, the high word is for K2 and the low word for K1.
  word keys_K1 = 0;
  word keys_K2 = 0;
  byte received;

  start();
  send(TM16XX_CMD_DATA_READ);		// send read buttons command
  for (int i = 0; i < 5; i++) {
  	received=receive();
    keys_K1 |= (( (received&_BV(0))     | ((received&_BV(3))>>2)) << (2*i));			// bit 0 for K1/KS1 and bit 3 for K1/KS2
    keys_K2 |= ((((received&_BV(1))>>1) | ((received&_BV(4))>>3)) << (2*i));			// bit 1 for K2/KS1 and bit 4 for K2/KS2
  }
  stop();
  return((uint32_t)keys_K2<<16 | (uint32_t)keys_K1);
}