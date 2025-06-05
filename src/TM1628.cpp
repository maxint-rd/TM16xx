/*
TM1628.cpp - Library for TM1628.

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment displays and use Adafruit GFX on matrix displays.

Made by Maxint R&D. See https://github.com/maxint-rd
*/

#include "TM1628.h"

TM1628::TM1628(byte dataPin, byte clockPin, byte strobePin, byte numDigits, bool activateDisplay, byte intensity)
  : TM16xx(dataPin, clockPin, strobePin, TM1628_MAX_POS, numDigits, activateDisplay, intensity)
{
  _maxSegments=13;

  // Display mode command setting
  // Upper two grid pins are shared with the upper two segment pins, so select appropriate mode based on numDigits
  if (numDigits <= 4) {
    sendCommand(TM16XX_CMD_MODE_4GRID); 
  } else if (numDigits == 5) {
    _maxSegments=12;
    sendCommand(TM16XX_CMD_MODE_5GRID);
  } else if (numDigits == 6) {
    _maxSegments=11;
    sendCommand(TM16XX_CMD_MODE_6GRID);
  } else {
    _maxSegments=10;
    sendCommand(TM16XX_CMD_MODE_7GRID);
  }

  clearDisplay();
  setupDisplay(activateDisplay, intensity);
}

void TM1628::bitDelay()
{
  delayMicroseconds(5);
  // When using a fast clock (such as ESP8266/ESP32) a delay is needed to read bits correctly.
  // NOTE: Testing on TM1628 reading keys shows that CLK should be slower than 50Khz.
  // Faster than 5us delay (44 Khz) worked unreliably.
  //
  // Testing of TM1628 on 3v3 ESP32 (TinyPico) showed reading okay with 5us delay (45`kHz CLK)
  // NOTE: Testing without recommended external pull-up 1K-10K on DIO failed. Connecting
  // DIO to Logic Analyzer made it work, as well as a 2K pull-up resistor.
  // Logic Analyzer settings: SPI LSB-first, Clock high when inactive, data valid on clock
  // trailing edge, enable line active low.
  // 
  // The datasheet specifies a maximum clock rate of 1MHz, but (unlike the TM1638) testing 
  // on the ESP8266 shows this appears to be too fast. For that reason the delay between reading bits should be more than 4us.
  // TODO: test medium speed MCUs like LGT8F328P (32Mhz) and STM32. Make this delay conditional on F_CPU.
}


void TM1628::setSegments(byte segments, byte position)
{   // set 8 leds on common grd as specified
  // TM1628 supports 10-13 segments in two bytes
  // For the 7-segment digit displays only the low byte (containing seg1-seg8) is sent to the display
  // When 13x4 mode is set, a 13-15 segment display is assumed and 16bit segments are sent.
  if(position<_maxDisplays)
  {
    if(_maxSegments<13)
        sendData(position << 1, segments);
    else
    {
      uint16_t segments16=segments;
      if(segments16&bit(6))
        segments16|=bit(8);   // duplicate G1 to G2
      this->setSegments16((uint16_t)segments16, position);
    }
  }
}

//void TM1628::setSegmentMap(const PROGMEM byte aMap[])
void TM1628::setSegmentMap(PGM_P aMap)  // OK???? on ESP32 core 3.0.2, tinyPico, works for  ESP8266
{ // Set a segment map to be used in subsequent setting of _maxSegments segments
  // aMap[] should be a 16-byte progmem array, each byte designating the target position of the segment.
  // Example map for easy breadboard wiring of TM1650 to dual 5241BS (TM1640 pin 1 right-top, 5241BS pin 1 left bottom)
  // const PROGMEM byte aSegmentMap[]={12, 11, 8, 6, 1, 9, 13, 7, 5, 10, 15, 14, 4, 3, 2, 0};  // mapping for TM1640 left of 5241BS with streight wiring
  // const PROGMEM byte aSegmentMap[]={0, 1, 2, 3, 4, 5, 6, 14, 7, 8, 9, 10, 13, 12, 11, 15};  // mapping for HT16K33 module with dual 5241AS 14-segment + DP
  // const PROGMEM byte aSegmentMap[]={3, 2, 12, 10, 6, 1, 9, 15 /*11*/, 9, 0, 7, 4, 8, 5, 11, 15};      // mapping for LED-display dual 5241AS combined 19-pins, having G1=G2,no DP to use 13 segments
  _pSegmentMap=aMap;
}

uint16_t TM1628::mapSegments16(uint16_t segments)
{ // Map the segments to another location if that's requested.
  // The segment map is a progmem array of 16 bytes long of which each byte specfies the remapped position.
  // Only segments 0-14 are actually used. Segment 15 is used to specify unsused segments.
  // Usually segment A is mapped to pin SEG0. Using segment mapping this can become any other pin.
  // The segment after mapping should use only _maxSegments, but this depends on the map.
  if(_pSegmentMap)
  {
    uint16_t nSegmentsMapped=0;
    for(byte n=0; n<16; n++)
      nSegmentsMapped|=((segments&bit(n))?bit(pgm_read_byte(_pSegmentMap+n)):0);
    segments=nSegmentsMapped;
  }
  return(segments);
}

void TM1628::setSegments16(uint16_t segments, byte position)
{ // method to send more than 8 segments (13 max for TM1628)
  // segments 1-8 are in bits 0-7 of position bytes 0,2,4,6,8,10,12
  // like TM1668 segments 9-13 are in bits 0-1 and 3-5 of position bytes 1,3,5,7,9,11,13
  // ????segments 9-13 are in bits 0-3 of position bytes 1,3,5,7,9,11,13
  if(position<_maxDisplays)
  {
    // Map segments if specified for alternative segment wiring.
    segments=mapSegments16(segments);
  
    sendData(position << 1, (byte)segments&0xFF);
    //sendData((position << 1) | 1, (byte)(segments>>8)&0x30);
    //sendData((position << 1) | 1, (byte)(segments>>8)&0xFF);
    sendData((position << 1) | 1, ((byte)(segments>>8)&0x03) | (((byte)(segments>>10)&0x07)<<3));
  }
}

void TM1628::clearDisplay()
{ // Clear all data registers. Overloaded from base to clear all segments
  for (int i = 0; i < _maxDisplays; i++) {
    setSegments16(0, i);
  }
}

void TM1628::sendAsciiChar(byte pos, char c, bool fDot, const byte font[])
{ // Method to send an Ascii character to the display.
  // This method is also called by TM16xxDisplay.print() to display characters.
  // The base class uses the default 7-segment font to find the LED pattern.
  // Derived classes for multi-segment displays or alternate layout displays can override this method.
  // Although TM1628 only supports 13 segments, we still use FONT_15SEG -> G2 is connected to G1 and DP is not used.
  uint16_t uSegments= pgm_read_word(TM16XX_FONT_15SEG+(c - 32));
  setSegments16(uSegments | (fDot ? 0b10000000 : 0), pos);

  // TODO: if display mode <13 chars use 7 segment font
}

uint32_t TM1628::getButtons(void)
{ // Keyscan data on the TM1628 is 2x10 keys, received as an array of 5 bytes (same as TM1668).
  // Of each byte the bits B0/B3 and B1/B4 represent status of the connection of K1 and K2 to KS1-KS10
  // Byte1[0-1]: KS1xK1, KS1xK2
  // The return value is a 32 bit value containing button scans for both K1 and K2, the high word is for K2 and the low word for K1.
  word keys_K1 = 0;
  word keys_K2 = 0;
  byte received;

  start();
  send(TM16XX_CMD_DATA_READ);       // send read buttons command
  for (int i = 0; i < 5; i++) {
    received=receive();
    keys_K1 |= (( (received&_BV(0))     | ((received&_BV(3))>>2)) << (2*i));            // bit 0 for K1/KS1 and bit 3 for K1/KS2
    keys_K2 |= ((((received&_BV(1))>>1) | ((received&_BV(4))>>3)) << (2*i));            // bit 1 for K2/KS1 and bit 4 for K2/KS2
  }
  stop();
  return((uint32_t)keys_K2<<16 | (uint32_t)keys_K1);
}