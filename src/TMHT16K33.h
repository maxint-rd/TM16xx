/*
TMHT16K33 - Library for HT16K33 led display driver using the TM16xx API.
HT16K33 uses I2C (default address 0x70). max 16seg x 8grid, 13x3 buttons not supported (yet).

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment or 14-segment displays.

Tested modules:
   QYF-0231: 0.54" 14-segment LED HT16K33 (VK16H33) Backpack, 4 digits, 14SEG+DP, no buttons, I2C default address 0x70

Made by Maxint R&D. See https://github.com/maxint-rd
*/

#ifndef TMHT16K33_h
#define TMHT16K33_h

#include "TM16xx.h"   // will include Arduino.h

#define TMHT16K33_MAX_POS 8

// Display init Commands
#define TMHT16K33_ON            0x21  // 0=off 1=on
#define TMHT16K33_STANDBY       0x20  // bit xxxxxxx0

// Display blink/show command: 1000 0xxy
// y    =  display on / off
// xx   =  00=off     01=2Hz     10=1Hz     11=0.5Hz
#define TMHT16K33_DISPLAYON     0x81
#define TMHT16K33_DISPLAYOFF    0x80
#define TMHT16K33_BLINKON0_5HZ  0x87
#define TMHT16K33_BLINKON1HZ    0x85
#define TMHT16K33_BLINKON2HZ    0x83
#define TMHT16K33_BLINKOFF      0x81

// Display brightness command: 1110 xxxx
// xxxx    =  0000 .. 1111 (0 - F)
#define TMHT16K33_BRIGHTNESS    0xE0

// Display address command: 0000 xxxx
// xxxx    =  0000 .. 1111 (0 - F)
// Two bytes per digit: First digit: seg0-7: 0x00, seg 8-15: 0x01
#define TMHT16K33_CMD_ADDRESS           0x00

// modes for setting interrupt pin and reading buttons are not supported (yet)


class TMHT16K33 : public TM16xx
{
  public:
    /** Instantiate a TMHT16K33 module specifying the data pin, number of digits */
    /** DEPRECATED: activation, intensity (0-7) and display mode are no longer used by constructor. */
    TMHT16K33(byte i2cAddress, byte sdaPin, byte sclPin, byte numDigits);
    TMHT16K33(byte i2cAddress, byte numDigits) : TMHT16K33(i2cAddress, 255, 255, numDigits) {};
    TMHT16K33(byte i2cAddress) : TMHT16K33(i2cAddress, 255, 255, 4) {};
    // TODO: remove deprecated parameters - TMHT16K33(byte i2cAddress, byte sdaPin, byte sclPin, byte numDigits=4);
    virtual void clearDisplay();
    virtual void setupDisplay(bool active, byte intensity);
    virtual void begin(bool activateDisplay=true, byte intensity=7);

	  virtual void setSegments(byte segments, byte position);   // will duplicate G to G1/G2 in 15-segment
	  virtual void setSegments16(uint16_t segments, byte position);   // some modules support more than 8 segments
    /** Set an Ascii character on a specific location (overloaded for 15-segment display) */
		virtual void sendAsciiChar(byte pos, char c, bool dot, const byte font[] = TM16XX_FONT_DEFAULT); // public method to allow calling from TM16xxDisplay

	  // Set mapping array to be used when displaying segments
	  // The array should contain _maxSegments bytes specifying the desired mapping
	  //virtual void setSegmentMap(const byte aMap[]);
	  virtual void setAlphaNumeric(bool fAlpha, const byte *aMap=NULL);  // PGM_P OK???? on ESP32 core 3.0.2, tinyPico, works for ESP8266 (PGM_P defined as const char * in pgmspace.h)

  protected:
    virtual void start();
    virtual void stop();
    virtual void send(byte data);
    virtual void sendData(byte address, byte data);
    virtual void sendData16(byte address, uint16_t data);
    virtual void sendCommand(byte cmd);

  private:
    bool isConnected();
		uint16_t mapSegments16(uint16_t segments);
    bool fAlphaNumeric=true;     // If true use 15-segment alphanumeric  display when having at least 13 SEG (15-seg: 14-segments plus dot)
    uint8_t _i2cAddress;
		//const PROGMEM byte *_pSegmentMap=NULL;               // pointer to segment map set using setSegmentMap()
		//PGM_P _pSegmentMap=NULL;               // pointer to segment map set using setSegmentMap() - PGM_P needed in ESP8266 core 3.0.2
    const byte *_pSegmentMap=NULL;               // pointer to segment map set using setSegmentMap() - PGM_P needed in ESP8266 core 3.0.2
};

// For alternative segment wiring, use setSegmentMap(). Example:   module.setSegmentMap(TMHT16K33_SEGMAP_QYF0231);
//const PROGMEM byte TMHT16K33_SEGMAP_QYF0231[]={0, 1, 2, 3, 4, 5, 6, 14, 7, 8, 9, 10, 13, 12, 11, 15};      // mapping for QYF-0231 HT16K33 module with dual 5241AS 14-segment + DP
const byte TMHT16K33_SEGMAP_QYF0231[] PROGMEM ={0, 1, 2, 3, 4, 5, 6, 14, 7, 8, 9, 10, 13, 12, 11, 15};      // mapping for QYF-0231 HT16K33 module with dual 5241AS 14-segment + DP

#endif
