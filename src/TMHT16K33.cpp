/*
TMHT16K33 - Library for HT16K33 led display driver using the TM16xx API.
HT16K33 uses I2C (default address 0x70). max 16seg x 8grid, 13x3 buttons not supported (yet).

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment or 14-segment displays.

Tested modules:
   QYF-0231: 0.54" 14-segment LED HT16K33 (VK16H33) Backpack, 4 digits, 14SEG+DP, no buttons, I2C default address 0x70

5241BS/5241AS - 2 digits of 14 segments plus dot:
   |---A---|
   |\  |  /|
   F H J K B
   |  \|/  |
   |-G1-G2-|
   |  /|\  |
   E N M L C 
   |/  |  \|
   |---D---| DP
 1  marked
   
Pinout LED-display marked 5241AS on module QYF0231 (bottom side up)
  | | | | | | | | |            Index Segment Pin             Index Segment Pin
  | | | | | | | | |            [0]         A  12             [ 8]       G2   6 
  1 2 3 4 5 6 7 8 9            [1]         B  10             [ 9]        H  17
+-|-|-|-|-|-|-|-|-|-+ <-marked [2]         C   9             [10]        J  15
| o o x o o o o o o |    side  [3]         D   7             [11]        K  14
|        .         .|          [4]         E   1             [12]        L   5
| 5241AS (Com.Cat.) |          [5]         F  18             [13]        M   4
| 5241BS (Com.An.)  |          [6]        G1  13             [14]        N   2
|                   |          [7]        DP   8
| o o v o o o o v o |
+-|-|-|-|-|-|-|-|-|-+       (x) Pin 3 is not connected
  1 1 1 1 1 1 1 1 1         (v) Anode/Cathode digit 1 is on pin 16
  8 7 6 5 4 3 2 1 0         (v) Anode/Cathode digit 2 is on pin 11
  | | | | | | | | |   

Note: LED-display KYX-5241AS has the same pinout, but NC pin 3 is missing.

Made by Maxint R&D. See https://github.com/maxint-rd
*/

#include "TMHT16K33.h"  // will include Arduino.h
#include <Wire.h>       // needed for I2C

TMHT16K33::TMHT16K33(byte i2cAddress, byte sdaPin, byte sclPin, byte numDigits)
	: TM16xx(sdaPin, sclPin, sdaPin, TMHT16K33_MAX_POS, numDigits)
{ // The HT16K33 has I2C using SDA and SCL on default address 0x70. In the parent-constructor DIN, CLK and STB are initialized, but not used.
  // On ESP32/ESP8266 sdaPin and sclPin can be specified to use alternative pins.
  // In begin() I2C is actually initialized.
  // DEPRECATED: activation, intensity (0-7) and display mode are no longer used by constructor.  
  _i2cAddress = i2cAddress;

  // NOTE: CONSTRUCTORS SHOULD NOT CALL DELAY() <= gives hanging on certain ESP8266 cores as well as on LGT8F328P
  // Using micros() or millis() in constructor also gave issues on LST8F328P.
  // TMHT16K33 uses bit timing to communicate, so clearDisplay() and setupDisplay() cannot be called in constructor.
  // Call begin() in setup() to clear the display and set initial activation and intensity.
  _maxSegments = 16;
}
/*
TODO: remove deprecated parameters
TMHT16K33::TMHT16K33(byte i2cAddress, byte sdaPin, byte sclPin, byte numDigits)
	: TM16xx(sdaPin, sclPin, sdaPin, TMHT16K33_MAX_POS, numDigits, true, 7)
*/

bool TMHT16K33::isConnected()
{ // check to see if the module is connected and responding
  Wire.beginTransmission(_i2cAddress);
  return (Wire.endTransmission() == 0);
}

void TMHT16K33::begin(bool activateDisplay, byte intensity)
{ // Call begin() in setup() to clear the display and set initial activation and intensity.
  // begin() is implicitly called upon first sending of display data, but only executes once.
  static bool fBeginDone=false;
  if(fBeginDone)
    return;
  fBeginDone=true;
  
  // Init i2c. Some architectures allow using different pins for I2C
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_CH32)
  if ((dataPin < 255) && (clockPin < 255))
  {
    Wire.begin(dataPin, clockPin);
  } else {
    Wire.begin();
  }
#else
  Wire.begin();
#endif
  Wire.setClock(100000L);     // NOTE: when setting a speed that's not supported 400k is used on ESP/LGT. Only 100k and 400k seem to be supported on LGT8F328P

  if(!isConnected())
  { // I2C connection failed!
    fBeginDone=false;
    return;
  }
  
  clearDisplay();
  setupDisplay(activateDisplay, intensity);
} 
  
void TMHT16K33::start()
{ // For the TMHT16K33, start and stop are sent using I2C protocol so no separate start or stop
}

void TMHT16K33::stop()
{ // For the TMHT16K33, start and stop are sent using I2C protocol so no separate start or stop
}

void TMHT16K33::send(byte data)
{	// Send a byte to the HT16K33 chip using I2C
	// Note: this method has no actual use since it's single byte only (sendCommand() is used for single byte commands)
  Wire.beginTransmission(_i2cAddress);
  Wire.write(data);
  Wire.endTransmission();
}

void TMHT16K33::sendData(byte address, byte data)
{	// Note: HT16K33 uses different commands than TM16XX chips
  begin();    // begin() is implicitly called upon first sending of display data, but only executed once.
  Wire.beginTransmission(_i2cAddress);
  Wire.write(TMHT16K33_CMD_ADDRESS | ((address&0x07)<<1));
  Wire.write(data);
  Wire.endTransmission();
}

void TMHT16K33::sendData16(byte address, uint16_t data)
{	// Note: HT16K33 uses different commands than TM16XX chips
  begin();    // begin() is implicitly called upon first sending of display data, but only executed once.
  Wire.beginTransmission(_i2cAddress);
  Wire.write(TMHT16K33_CMD_ADDRESS | ((address&0x07)<<1));
  Wire.write(data&0x00FF);    // data address increments automaticaly, first write segments 0-7
  Wire.write(data>>8);        // then write segments 8-15
  Wire.endTransmission();
}

void TMHT16K33::sendCommand(byte cmd)
{ // Send a display command. Note: HT16K33 uses different commands than TM16XX chips
  Wire.beginTransmission(_i2cAddress);
  Wire.write(cmd);
  Wire.endTransmission();
}

void TMHT16K33::clearDisplay()
{	// Clear all data registers. The number of registers is 0x00 to 0x0F on HT16K33 for COM0-COM7 x SEG0-SEG15.
	for (int i = 0; i < _maxDisplays; i++) {
    sendData16(i, 0);
  }
}

void TMHT16K33::setupDisplay(bool active, byte intensity)
{	// HT16K33 allows setting the duty cycle in 16 levels. For uniformity within TM16xx, intensity is translated to 0-7 (low to high).
  // Note that the distribution of these  8 levels is sligthly different from the uneven distribution of many TM16xx chips.
  sendCommand(active ? TMHT16K33_ON : TMHT16K33_STANDBY);
  sendCommand(active ? TMHT16K33_DISPLAYON : TMHT16K33_DISPLAYOFF);
  sendCommand(TMHT16K33_BRIGHTNESS | (((intensity&0x07)<<1)|0x01));
}


void TMHT16K33::setSegments(byte segments, byte position)
{ // Set regular 7-segment segments on the 15-segment display. This allows for using the regular 7-segment font.
  // On the 5241BS 15-segment display, segment G is split into G1 and G2. Assume these to be segments 6 and 8, having DP on segment 7 (index 0)
  // In setSegments16() the segments may be mapped to a different segment order.
  uint16_t segments16=segments;
  if(segments16&bit(6))
    segments16|=bit(8);   // duplicate G1 to G2
  this->setSegments16((uint16_t)segments16, position);
}


//void TMHT16K33::setAlphaNumeric(bool fAlpha, const PROGMEM byte aMap[])
void TMHT16K33::setAlphaNumeric(bool fAlpha, const byte *aMap)  // PGM_P OK???? on ESP32 core 3.0.2, tinyPico, works for  ESP8266
{ // Set a segment map to be used in subsequent setting of _maxSegments segments
  // Example map for easy breadboard wiring of TM1650 to 5241BS (TM1640 pin 1 right-top, 5241BS pin 1 left bottom)
  // byte aSegmentMap[]={12, 11, 8, 6, 1, 9, 13, 7, 5, 10, 15, 14, 4, 3, 2, 0};  // mapping for TM1640 left of 5241BS with streight wiring
  // byte aSegmentMap[]={0, 1, 2, 3, 4, 5, 6, 14, 7, 8, 9, 10, 13, 12, 11, 15};  // mapping for HT16K33 module with dual 5241AS 14-segment + DP
  this->fAlphaNumeric=fAlpha;
  if(aMap)
    _pSegmentMap=aMap;
}

uint16_t TMHT16K33::mapSegments16(uint16_t segments)
{ // Map the segments to another location if that's requested.
  // The segment map is an array of _maxSegments long of which each element specfies the remapped position.
  // Usually segment A is mapped to pin SEG0. Using segmentmapping this can become any other pin.
  if(_pSegmentMap)
  {
    uint16_t nSegmentsMapped=0;
    for(byte n=0; n<_maxSegments; n++)
      nSegmentsMapped|=((segments&bit(n))?bit(pgm_read_byte(_pSegmentMap+n)):0);
//      nSegmentsMapped|=((segments&bit(n))?bit(_pSegmentMap[n]):0);
    segments=nSegmentsMapped;
  }
  return(segments);
}


void TMHT16K33::setSegments16(uint16_t segments, byte position)
{	// Set 16 leds on common grd as specified

  // Map segments if specified for alternative segment wiring.
  segments=mapSegments16(segments);

  // Send the segment data
	if(position<_maxDisplays)
		sendData16(position, segments);
}

void TMHT16K33::sendAsciiChar(byte pos, char c, bool fDot, const byte font[])
{ // Method to send an Ascii character to the display.
  // This method is also called by TM16xxDisplay.print() to display characters.
  // The base class uses the default 7-segment font to find the LED pattern.
  // Derived classes for multi-segment displays or alternate layout displays can override this method.
  if(fAlphaNumeric)
  {
    uint16_t uSegments= pgm_read_word(TM16XX_FONT_15SEG+(c - 32));   // TM16XX_FONT_15SEG

    if(this->flipped)
    { // Flip the character 180 degrees by some clever bit-manipulation.
      // For 7 segments: swap segment A, B and C with D, E and F; leave G and DP intact
      // (see PR #58 and issue #20 for info and references)
      //   byte xored = (data ^ (data >> 3)) & (7);
      //   data = data ^ (xored | (xored << 3));
      //   pos = this->digits - 1 - pos;
      
      // For 14-segments: a,b,c should be swapped with d,e,f, then g1 should be swapped with g2, 
      //   then h,j,k should be swapped with l,m,n and the dot be left behind.
      // You can split the variable into 3 parts:
      //    & 0x0E07 (0000 1110 0000 0111: abc and hjk)
      //    & 0x7038 (0111 0000 0011 1000: def and lmn)
      //    & 0x01C0 (0000 0001 1100 0000: dot, g1 and g2)
      // Shift the first part to left by 3 bits, Shift the second part to right by 3 bits, swap g1 and g2 in the third part, then OR all the parts together.
      // (Bit swapping: https://graphics.stanford.edu/~seander/bithacks.html#SwappingBitsXOR     @HoseanRC - Thank you!)
      //
      uint16_t part1=uSegments & 0x0E07; // (abc and hjk)
      uint16_t part2=uSegments & 0x7038; // (def and lmn)
      uint16_t part3=uSegments & 0x01C0; // (dot, g1 and g2)
      uint16_t  xored= ((part3 >> 6) ^ (part3 >> 8)) & 1;
      part3 = part3 ^ ((xored << 6) | (xored << 8));
      uSegments = part1<<3 | part2>>3 | part3;
      pos = this->digits - 1 - pos;
    }
    setSegments16(uSegments | (fDot ? 0b10000000 : 0), pos);
  }
  else
    TM16xx::sendAsciiChar(pos, c, fDot, font);    // the base method calls sendChar, which will do 7-segment flipping if needed
}