/*
TM1618 - Library for TM1618 led display driver.
TM1618: 5x7 - 8x4 SEGxGRD, 5 x 1 single button, DIO/CLK/STB

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment displays and use Adafruit GFX on matrix displays.

Made by Maxint R&D. See https://github.com/maxint-rd

TM1618 Pinout:
             +----v----+
        DIO -|1      18|- GRD1
        CLK -|2      17|- GRD2
        STB -|3      16|- GND
         K2 -|4      15|- GRD3
        VDD -|5      14|- GRD4
   SEG1/KS1 -|6      13|- SEG14/GRD5
   SEG2/KS2 -|7      12|- SEG13/GRD6
   SEG3/KS3 -|8      11|- SEG12/GRD7
   SEG4/KS4 -|9      10|- SEG5/KS5
             +---------+
*/

#include "TM1618.h"

TM1618::TM1618(byte dataPin, byte clockPin, byte strobePin, byte numDigits, bool activateDisplay, byte intensity)
	: TM16xx(dataPin, clockPin, strobePin, TM1618_MAX_POS, numDigits, activateDisplay, intensity)
{
  // TM1618 has 4 display modes (5x7 - 8x4 SEGxGRD)
  // The upper 3 segment pins are shared with the upper grid pins, so select appropriate mode based on numDigits
  _maxSegments=8;
  if (numDigits > 4)
    _maxSegments=12-numDigits;

  // NOTE: CONSTRUCTORS SHOULD NOT CALL DELAY() <= gives hanging on certain ESP8266/ESP32 cores as well as on LGT8F328P 
  // Using micros() or millis() in constructor also gave issues on LST8F328P and CH32V003.
  // Some TM16xx chips uses bit-timing to communicate, so clearDisplay() and setupDisplay() cannot be called in constructor.
  // Call begin() in setup() to clear the display and set initial activation and intensity.
  // To make begin() optional, sendData() implicitely calls begin(). Although it will execute only once, it may cause a
  // second call to clearDisplay() by an explicit call to clearDisplay(). Therefor sketches should preferably call begin().
}

void TM1618::setupDisplay(bool active, byte intensity)
{
  // Set display mode
  // Upper grid pins are shared with the upper segment pins, so select appropriate mode based on _maxSegments
  sendCommand(_maxSegments==5 ? TM16XX_CMD_MODE_7GRID : (_maxSegments==6 ? TM16XX_CMD_MODE_6GRID : (_maxSegments==7 ? TM16XX_CMD_MODE_5GRID : TM16XX_CMD_MODE_4GRID)));

  // Switch display on/off and set intensity 
  sendCommand(TM16XX_CMD_DISPLAY | (active ? 8 : 0) | min(7, intensity));
}

void TM1618::setSegments(byte segments, byte position)
{	// Set 8 leds on common grd as specified.
  // TM1618 does not have SEG6-SEG8 pins, but instead has SEG12-SEG14.
  // TM1618 uses up to 8 segments in two bytes
  // The first/lower byte has SEG1-5 in bits 0-4, the second/upper has SEG6-8 in bits 12-14
  if(position<_maxDisplays)
  {
    sendData(position << 1, (byte)segments&0x1F);
    sendData((position << 1) | 1, (byte)(segments>>5)<<3);
  }
}

void TM1618::clearDisplay()
{ // NOTE: TM16xx class assumes chips only have 2 bytes per digit when it uses >8 segments
  for(byte nPos=0; nPos<TM1618_MAX_POS; nPos++)
  {
	  // all OFF
	  sendData(nPos << 1, 0);
	  sendData((nPos << 1) | 1, 0);
  }
}

uint32_t TM1618::getButtons(void)
{	// Keyscan data on the TM1618 is 1x5 keys, received as an array of 3 bytes (similar to TM1630).
  // Of each byte the bits B1/B4 represent status of the connection of K2 to KS1-KS5
  // The return value is a 32 bit value containing button scans for K2, the low word is for K2 and the high word is empty
  word keys_K2 = 0;
  byte received;

  start();
  send(TM16XX_CMD_DATA_READ);		// send read buttons command
  bitDelay();  // the TM1618 datasheet specifies a Twait of minimal. 1us.
  for (int i = 0; i < 3; i++) {
    received=receive();
    keys_K2 |= ((((received&_BV(1))>>1) | ((received&_BV(4))>>3)) << (2*i));			// bit 1 for K2/KS1 and bit 4 for K2/KS2
  }
  stop();
  return((uint32_t)keys_K2);
}