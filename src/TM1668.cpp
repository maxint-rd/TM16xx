/*
TM1668.cpp - Library implementation for TM1668.

Copyright (C) 2011 Ricardo Batista (rjbatista <at> gmail <dot> com)
Adjusted for TM1668 by Maxint R&D, based on TM1638 code. See https://github.com/maxint-rd/

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

#include "TM1668.h"
#include "string.h"

TM1668::TM1668(byte dataPin, byte clockPin, byte strobePin, boolean activateDisplay, byte intensity, byte numDigits, byte displaymode)
	: TM16xx(dataPin, clockPin, strobePin, 7, numDigits, activateDisplay, intensity)		// "numDigits" is the number of digits
{
	// set the display mode
  sendCommand(displaymode);							// default TM1668_DISPMODE_7x10: display mode 7 Grid x 10 Segment

  switch(displaymode)
  {
  case TM1668_DISPMODE_4x13:
  	_maxDisplays=4;
  	break;
  case TM1668_DISPMODE_5x12:
  	_maxDisplays=5;
  	break;
  case TM1668_DISPMODE_6x11:
  	_maxDisplays=6;
  	break;
  case TM1668_DISPMODE_7x10:
  default:
  	_maxDisplays=7;
  	break;
  }
  _maxSegments=17-_maxDisplays;

	clearDisplay();
	setupDisplay(activateDisplay, intensity);
}

/*
void TM1668::setDisplayToHexNumber(unsigned long number, byte dots, boolean leadingZeros,
	const byte numberFont[])
{
  for (int i = 0; i < digits; i++) {
	if (!leadingZeros && number == 0) {
		clearDisplayDigit(digits - i - 1, (dots & (1 << i)) != 0);
	} else {
		setDisplayDigit(number & 0xF, digits - i - 1, (dots & (1 << i)) != 0, numberFont);
		number >>= 4;
    }
  }
}

void TM1668::setDisplayToDecNumberAt(unsigned long number, byte dots, byte startingPos, boolean leadingZeros,
	const byte numberFont[])
{
  if (number > 99999999L) {
    setDisplayToError();
  } else {
    for (int i = 0; i < digits - startingPos; i++) {
      if (number != 0) {
        setDisplayDigit(number % 10, digits - i - 1, (dots & (1 << i)) != 0, numberFont);
        number /= 10;
      } else {
		if (leadingZeros) {
		  setDisplayDigit(0, digits - i - 1, (dots & (1 << i)) != 0, numberFont);
		} else {
		  clearDisplayDigit(digits - i - 1, (dots & (1 << i)) != 0);
		}
      }
    }
  }
}

void TM1668::setDisplayToDecNumber(unsigned long number, byte dots, boolean leadingZeros,
	const byte numberFont[])
{
	setDisplayToDecNumberAt(number, dots, 0, leadingZeros, numberFont);
}

void TM1668::setDisplayToSignedDecNumber(signed long number, byte dots, boolean leadingZeros,
		const byte numberFont[])
{
	if (number >= 0) {
		setDisplayToDecNumberAt(number, dots, 0, leadingZeros, numberFont);
	} else {
		if (-number > 9999999L) {
		    setDisplayToError();
		} else {
			setDisplayToDecNumberAt(-number, dots, 1, leadingZeros, numberFont);
			sendChar(0, MINUS, (dots & (0x80)) != 0);
		}
	}
}

void TM1668::setDisplayToBinNumber(byte number, byte dots, const byte numberFont[])
{
  for (int i = 0; i < digits; i++) {
    setDisplayDigit((number & (1 << i)) == 0 ? 0 : 1, digits - i - 1, (dots & (1 << i)) != 0, numberFont);
  }
}
*/

void TM1668::setLED(byte color, byte pos)
{
    sendData((pos << 1) + 1, color);
}

void TM1668::setLEDs(uint32_t leds)
{
  for (int i = 0; i < digits; i++) {
    byte color = 0;

    if ((leds & (1 << i)) != 0) {
      color |= TM1668_COLOR_RED;
    }

    if ((leds & (1 << (i + 8))) != 0) {
      color |= TM1668_COLOR_GREEN;
    }

    if ((leds & (1 << (i + 16))) != 0) {
      color |= TM1668_COLOR_BLUE;
    }

    setLED(color, i);
  }
}


void TM1668::setRGBLEDs(uint32_t uRgbLeds)
{	// Common anode RGB LEDs can be connected on the 10 segments and GRID5-7
	// for each color there are 3 segments and 10 leds
	
	word wSegmentsR=0;
	word wSegmentsG=0;
	word wSegmentsB=0;

  _uTenRgbLeds=uRgbLeds;
	
	// convert uRgbLeds of 
	//Serial.println("--------\nsetRGBLEDs u:");
	for(int i=0; i<10; i++)
	{
		wSegmentsR|=((uRgbLeds&_BV(0))   )<<i;
		wSegmentsG|=((uRgbLeds&_BV(1))>>1)<<i;
		wSegmentsB|=((uRgbLeds&_BV(2))>>2)<<i;
		uRgbLeds>>=3;
  	//Serial.print("setRGBLEDs u");
  	//Serial.print(i);
  	//Serial.print(": ");
  	//Serial.println(uRgbLeds, BIN);
	}

/*
	Serial.print("setRGBLEDs R:");
	Serial.println(wSegmentsR, HEX);
	Serial.print("setRGBLEDs G:");
	Serial.println(wSegmentsG, HEX);
	Serial.print("setRGBLEDs B:");
	Serial.println(wSegmentsB, HEX);
*/
  sendData((TM1668_POS_RED << 1), wSegmentsR&0x00FF);
  sendData((TM1668_POS_RED << 1) + 1, wSegmentsR>>8);
  sendData((TM1668_POS_GREEN << 1), wSegmentsG&0x00FF);
  sendData((TM1668_POS_GREEN << 1) + 1, wSegmentsG>>8);
  sendData((TM1668_POS_BLUE << 1), wSegmentsB&0x00FF);
  sendData((TM1668_POS_BLUE << 1) + 1, wSegmentsB>>8);
/*
  Serial.print("--------\nsetRGBLEDs _uTenRgbLeds:");
  Serial.println(_uTenRgbLeds, BIN);
*/
}

void TM1668::setRGBLED(byte color, byte pos)
{	// set one specific led to a predefined color
	//uTenRgbLeds^= (color<<(pos*3));
	_uTenRgbLeds&= ~(0B111<<(pos*3));			// clear old value for pos (leaving others unchanged)
	_uTenRgbLeds|= (color<<(pos*3));				// set new value for pos (leaving others unchanged)
/*
  Serial.print("--------\n");
  Serial.print("setRGBLED c");
 	Serial.print(color);
  Serial.print("(0B");
 	Serial.print(color, BIN);
  Serial.print("), p");
 	Serial.print(pos);
  Serial.print(": ");
  Serial.print("setRGBLED uTenRgbLeds:");
  Serial.println(_uTenRgbLeds, BIN);
*/
  setRGBLEDs(_uTenRgbLeds);
}

uint32_t TM1668::getButtons(byte keyset)		// keyset=TM1668_KEYSET_ALL
{	// Keyscan data on the TM1668 is an array of 5 bytes (whereas TM1638 uses four bytes, six bits/byte).
	// Of each byte the bits B0/B3 and B1/B4 represent status of the connection of K1 and K2 to KS1-KS10
	// Byte1[0-1]: KS1xK1, KS1xK2
	// The return value is a 32 bit value containing either button scans for K1 or K2 or for both K1 and K2.
	// If both are returned, the high word is for K2 and the low word for K1.
  word keys_K1 = 0;
  word keys_K2 = 0;
  byte received;

  digitalWrite(strobePin, LOW);
  send(0x42);		// send read buttons command
  for (int i = 0; i < 5; i++) {
  	received=receive();
    keys_K1 |= ( received&_BV(0)     | (received&_BV(3))>>2) << (2*i);			// bits 0 and	3 for K1/KS1 and K1/KS2
    keys_K2 |= ((received&_BV(1))>>1 | (received&_BV(4))>>3) << (2*i);			// bits 1 and	4 for K2/KS1 and K2/KS2
  }
  digitalWrite(strobePin, HIGH);
	if(keyset==TM1668_KEYSET_K1)
		return(keys_K1);
	else if(keyset==TM1668_KEYSET_K2)
		return(keys_K2);
  return(keys_K1|(keys_K2<<16));
}

/**/
void TM1668::sendChar(byte pos, byte data, boolean dot)
{ // Send data for a character to the display
	// TM1668 uses 10-13 segments in two bytes, similar to TM1638
	// Only the LSB (SEG1-8) is sent to the display
	sendData(pos << 1, data | (dot ? 0b10000000 : 0));		// TM1668 uses 10-13 segments in two bytes (similar to TM1638)
}
/**/
