/*
TM16xx.h - Library for TM1637, TM1638 and similar chips.
Modified by Maxint R&D. See https://github.com/maxint-rd/

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

#include "TM16xx.h"
//#include "string.h"

TM16xx::TM16xx(byte dataPin, byte clockPin, byte strobePin, byte maxDisplays, byte nDigitsUsed, boolean activateDisplay,	byte intensity)
{
  this->dataPin = dataPin;
  this->clockPin = clockPin;
  this->strobePin = strobePin;
  this->_maxDisplays = maxDisplays;
  this->digits = nDigitsUsed;

  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(strobePin, OUTPUT);

  digitalWrite(strobePin, HIGH);
  digitalWrite(clockPin, HIGH);

  //sendCommand(TM16XX_CMD_DISPLAY | (activateDisplay ? 8 : 0) | min(7, intensity));		// display command: on or intensity

/*
  sendCommand(TM16XX_CMD_DATA_AUTO);			// data command: set data mode to auto-increment write mode
	start();
  send(TM16XX_CMD_ADDRESS);					// address command + address C0H
  for (int i = 0; i < 16; i++) {		// TM1638 and TM1640 have 16 data addresses, TM1637 and TM1668 have less, but will wrap.
    send(0x00);
  }
	stop();
*/	
	// Note: calling these methods should be done in constructor of derived class in order to use properly initialized members!
/*
	clearDisplay();
	setupDisplay(activateDisplay, intensity);
*/
}

void TM16xx::setupDisplay(boolean active, byte intensity)
{
  sendCommand(TM16XX_CMD_DISPLAY | (active ? 8 : 0) | min(7, intensity));
}

void TM16xx::clearDisplay()
{	// Clear all data registers. The number of registers depends on the chip.
	// TM1638 (10x8): 10 segments per grid, stored in two bytes. The first byte contains the first 8 display segments, second byte has seg9+seg10  => 16 bytes
	// TM1640 (8x16): one byte per grid => 16 bytes
	// TM1637 (8x6): one byte per grid => 6 bytes
	// TM1668 (10x7 - 14x3): two bytes per grid => 14 bytes
  sendCommand(TM16XX_CMD_DATA_AUTO);		// set auto increment addressing mode

	// send the address followed by bulk-sending of the data to clear the display memory
	start();
  send(TM16XX_CMD_ADDRESS);
  for (int i = 0; i < _maxDisplays; i++) {
    send(0x00);
    if(_maxSegments>8)
    	send(0x00);		// send second byte (applicable to TM1638 and TM1668)
  }
	stop();

}

void TM16xx::setSegments(byte segments, byte position)
{	// set 8 leds on common grd as specified
	// TODO: support 10-14 segments on chips like TM1638/TM1668
	if(position<_maxDisplays)
		sendData(position, segments);
		//sendData(TM16XX_CMD_ADDRESS | position, segments);
}

void TM16xx::setSegments16(uint16_t segments, byte position)
{	// Some modules support more than 8 segments (e.g. 10 max for TM1638)
  // The position of the additional segments in the second data byte can be different per module,
  // For that reason this module has no implementation in the base class.
  // E.g. for TM1638/TM1668 segments 8-9 are in bits 0-1, for TM1630 segment 14 is in bit 5
  // This method assumes segments 0-7 to be in the lower byte and the extra segments in the upper byte
  // Depending on the module this method should shift the segments to the proper data position.
}


void TM16xx::sendChar(byte pos, byte data, boolean dot)
{
/*
	if(pos<_maxDisplays)
	  sendData(pos, data | (dot ? 0b10000000 : 0));
*/
	setSegments(data | (dot ? 0b10000000 : 0), pos);
}

void TM16xx::sendAsciiChar(byte pos, char c, boolean fDot)
{ // Method to send an Ascii character to the display
  // This method is also called by TM16xxDisplay.print to display characters
  // The base class uses the default 7-segment font to find the LED pattern.
  // Derived classes for multi-segment displays or alternate layout displays can override this method
  sendChar(pos, pgm_read_byte_near(TM16XX_FONT_DEFAULT+(c - 32)), fDot);
}


void TM16xx::setDisplayDigit(byte digit, byte pos, boolean dot, const byte numberFont[])
{
  sendChar(pos, pgm_read_byte_near(numberFont + (digit & 0xF)), dot);
}

void TM16xx::setDisplayToDecNumber(int nNumber, byte bDots)		// byte bDots=0
{	// Function to display a decimal number on a n-digit clock display.
	// Kept simple to fit in ATtiny44A
	// For extended display features use the TM16xxDisplay class

	// TODO: support large displays such as 8segx16 on TM1640
  for(byte nPos=1; nPos<=digits; nPos++)
  {
    setDisplayDigit(nNumber % 10, digits - nPos, bDots&_BV(nPos));
    nNumber/=10;
  }
}

void TM16xx::clearDisplayDigit(byte pos, boolean dot)
{
  sendChar(pos, 0, dot);
}

void TM16xx::setDisplay(const byte values[], byte size)
{	// send an array of values to the display
  for (byte i = 0; i < size; i++) {
    sendChar(i, pgm_read_byte_near(values+i), 0);
  }
}

void TM16xx::setDisplayToString(const char* string, const word dots, const byte pos, const byte font[])
{
  for (int i = 0; i < digits - pos; i++) {
  	if (string[i] != '\0') {
		  //sendChar(i + pos, pgm_read_byte_near(font+(string[i] - 32)), (dots & (1 << (digits - i - 1))) != 0);
  	  sendAsciiChar(i + pos, string[i], (dots & (1 << (digits - i - 1))) != 0);
		} else {
		  break;
		}
  }
}

byte TM16xx::getNumDigits()
{	// get the number of digits used (needed by TM16xxDisplay to combine modules)
  return(digits);
}

// key-scanning method, implemented in chip specific derived class
uint32_t TM16xx::getButtons()
{	// return state of up to 32 keys.
	return(0);
}


//
//  Protected methods
//

void TM16xx::bitDelay()
{	// if needed derived classes can add a delay (eg. for TM1637)
	//delayMicroseconds(50);

  // On fast MCUs like ESP32 a delay is required, especially when reading buttons
  // The TM1638 datasheet specifies a max clock speed of 1MHz. 
  // Testing shows that without delay the CLK line exceeds 1.6 MHz on the ESP32. While displaying data still worked, reading buttons failed.
  // Adding a 1us delay gives clockpulses of about 1.75us-2.0us (~ 250kHz) on an ESP32 @240MHz and a similar delay on an ESP8266 @160Mhz.
  // An ESP32 running without delay at 240MHz gave a CLK of  ~0.3us (~ 1.6MHz)
  // An ESP8266 running without delay at 160MHz gave a CLK of  ~0.9us (~ 470kHz)
  // An ESP8266 running without delay  at 80MHz gave a CLK of  ~1.8us (~ 240kHz)
	#if F_CPU>100000000
  	delayMicroseconds(1);
  #endif
}

void TM16xx::start()
{	// if needed derived classes can use different patterns to start a command (eg. for TM1637)
  digitalWrite(strobePin, LOW);
  bitDelay();
}

void TM16xx::stop()
{	// if needed derived classes can use different patterns to stop a command (eg. for TM1637)
  digitalWrite(strobePin, HIGH);
  bitDelay();
}

void TM16xx::send(byte data)
{
	// MMOLE 180203: shiftout does something, but is not okay (tested on TM1668)
	//shiftOut(dataPin, clockPin, LSBFIRST, data);
  for (int i = 0; i < 8; i++) {
    digitalWrite(clockPin, LOW);
    bitDelay();
    digitalWrite(dataPin, data & 1 ? HIGH : LOW);
    bitDelay();
    data >>= 1;
    digitalWrite(clockPin, HIGH);
    bitDelay();
  }
  bitDelay();		// NOTE: TM1638 specifies a Twait between bytes of minimal 1us.
}

void TM16xx::sendCommand(byte cmd)
{
	start();
  send(cmd);
  stop();
}

void TM16xx::sendData(byte address, byte data)
{
  sendCommand(TM16XX_CMD_DATA_FIXED);							// use fixed addressing for data
	start();
  send(TM16XX_CMD_ADDRESS | address);						// address command + address
  send(data);
  stop();
}

byte TM16xx::receive()
{
  byte temp = 0;

  // Pull-up on
  pinMode(dataPin, INPUT);
  digitalWrite(dataPin, HIGH);

  for (int i = 0; i < 8; i++) {
    temp >>= 1;

    digitalWrite(clockPin, LOW);
    bitDelay();		// NOTE: on TM1637 reading keys should be slower than 250Khz (see datasheet p3)

    if (digitalRead(dataPin)) {
      temp |= 0x80;
    }

    digitalWrite(clockPin, HIGH);
    bitDelay();
  }

  // Pull-up off
  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, LOW);

  return temp;
}
