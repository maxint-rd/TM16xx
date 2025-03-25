/*
TM16xx.cpp - Library for TM1637, TM1638 and similar chips.
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

#include "TM16xx.h"

TM16xx::TM16xx(byte dataPin, byte clockPin, byte strobePin, byte maxDisplays, byte nDigitsUsed, bool activateDisplay,	byte intensity)
{
  // DEPRECATED: activation, intensity (0-7) and display mode are no longer used by constructor.  
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

  // NOTE: CONSTRUCTORS SHOULD NOT CALL DELAY() <= gives hanging on certain ESP8266/ESP32 cores as well as on LGT8F328P 
  // Using micros() or millis() in constructor also gave issues on LST8F328P and CH32V003.
  // Some TM16xx chips uses bit-timing to communicate, so clearDisplay() and setupDisplay() cannot be called in constructor.
  // Call begin() in setup() to clear the display and set initial activation and intensity.
  // To make begin() optional, sendData() implicitely calls begin(). Although it will execute only once, it may cause a
  // second call to clearDisplay() by an explicit call to clearDisplay(). Therefor sketches should preferably call begin().
}

void TM16xx::setupDisplay(bool active, byte intensity)
{ // Set the display intensity and switch it on/off
  // Some TM16xx classes (e.g. TM1650) use _maxSegments in setupDisplay() to also set the display mode.
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
    if(_maxSegments>8)    // TODO: some chips (e.g. TM1618) have 8-segments or less, but still use two bytes!
    	send(0x00);		// send second byte (applicable to TM1638 and TM1668)
  }
	stop();

}

void TM16xx::begin(bool activateDisplay, byte intensity)
{ // Call begin() in setup() to clear the display and set initial activation and intensity.
  // begin() is implicitly called upon first sending of display data, but only executes once.
  // Some chips may require begin() to initialize communication or other things.
  //static bool fBeginDone=false; // NOTE: Statics are shared like globals when using multiple object instances. That's unwanted behavior here!
  if(fBeginDone)
    return;
  fBeginDone=true;
  clearDisplay();
  setupDisplay(activateDisplay, intensity);
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

void TM16xx::sendChar(byte pos, byte data, bool dot)
{
/*
	if(pos<_maxDisplays)
	  sendData(pos, data | (dot ? 0b10000000 : 0));
*/
  if(this->reversed)
    pos=this->digits - 1 - pos;
  if(this->flipped)
  { // Flip the character 180 degrees by some clever bit-manipulation.
    // (see PR #58 and issue #20 for info and references)
    // NOTE: for now only support for 7-segment characters
    // NOTE: The dot (DP) will change position too (from after a character to before; right-bottom to left-top)
    byte xored = (data ^ (data >> 3)) & (7);
    data = data ^ (xored | (xored << 3));
  }
  setSegments(data | (dot ? 0b10000000 : 0), this->flipped ? this->digits - 1 - pos : pos);
}

void TM16xx::sendAsciiChar(byte pos, char c, bool fDot, const byte font[])
{ // Method to send an Ascii character to the display
  // This method is also called by TM16xxDisplay.print to display characters
  // The base class uses the default 7-segment font to find the LED pattern.
  // Derived classes for multi-segment displays or alternate layout displays can override this method
  sendChar(pos, pgm_read_byte_near(font+(c - 32)), fDot);
}

void TM16xx::setDisplayFlipped(bool fFlipped)
{ // Set flipped state of the display (every digit is rotated 180 degrees)
  // Note: this only changes subsequent displayed characters, not the current 
  this->flipped = fFlipped;
}

void TM16xx::setDisplayReversed(bool fReversed)
{ // Set reversed state of the display (digits use reversed position, first is last and vise versa)
  // Note: this only changes subsequent displayed characters, not the current 
  this->reversed = fReversed;
}


void TM16xx::setDisplayDigit(byte digit, byte pos, bool dot, const byte numberFont[])
{
  sendChar(pos, pgm_read_byte_near(numberFont + (digit & 0xF)), dot);
}

void TM16xx::setDisplayToDecNumber(int nNumber, byte bDots, bool fLeadingZeros)		// byte bDots=0, bool fLeadingZeros=true
{	// Function to display a decimal number on a n-digit clock display.
	// Kept simple to fit in ATtiny44A
	// For extended display features use the TM16xxDisplay class

	// TODO: support large displays such as 8segx16 on TM1640
  // TODO: how about negative numbers?
  for(byte nPos=0; nPos<digits; nPos++)
  {
    if(nPos==0 || nNumber>0 || fLeadingZeros)
      setDisplayDigit(nNumber % 10, digits - 1 - nPos, bDots&_BV(nPos));
    else
      sendChar(digits - 1 - nPos, 0, bDots&_BV(nPos));
    nNumber/=10;
  }
}

void TM16xx::clearDisplayDigit(byte pos, bool dot)
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
  	  sendAsciiChar(i + pos, string[i], (dots & (1 << (digits - i - 1))) != 0, font);   // use sendAsciiChar to support 14-segments (via derived classes)
		} else {
		  break;
		}
  }
}

byte TM16xx::getNumDigits()
{	// get the number of digits used (needed by TM16xxDisplay to combine modules)
  return(digits);
}

void TM16xx::setNumDigits(byte numDigitsUsed)
{	// Set the number of digits used.
  // Used by setDisplayToDecNumber() and setDisplayToString() to right align
  // Also used by TM16xxDisplay to combine modules.
  this->digits=numDigitsUsed;

  // The maximum number of digits _maxDisplays is now set by a parameter in the constructor.
  // Some TM16xx chips support multiple display modes. Derived classes use _maxDisplays
  // or _maxSegments to set the display mode when setupDisplay() is called.
  // Those classes could override setNumDigits() to change these maxima.
  // (if numDigitsUsed != _maxDisplays change display mode and adjust maxima)
  // Unfortunately this doesn't work for chips like TM1650, which has TM1650_DISPMODE_4x8
  // and TM1650_DISPMODE_4x7. Mode 4x7 is to allow the DP/KP pin to signal key presses.
  // At the moment that class still uses a display mode parameter in the constructor.
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
	//#if F_CPU>40000000    // semi-fast processeors like CH32V003 @48Mhz may also need some delay
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
  begin();    // begin() is implicitly called upon first sending of display data, but executes only once.
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
