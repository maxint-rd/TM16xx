/*
TM1680 - Library for TM1680 led display driver using the TM16xx API.
TM1680 uses I2C where A0 needs to be pulled low to match 7-bit I2C address 0x73. Alternative I2C address 0x72
can be selected by pulling A1 low. (A0 high doesn't work with Arduino I2C, see TM16xx issue #2 for details).
TM1680 offers 24seg x 16grd or 32seg x 8grd (24-3224Px8Nx8M CC/CA), no buttons.

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment or 14-segment displays.

Tested displays:
   2 x 5241BS: 0.54" dual digit CA 14-segment plus dot , no buttons, I2C address 0x73 (A0 tied low)

Made by Maxint R&D. See https://github.com/maxint-rd
*/

#ifndef TM1680_h
#define TM1680_h

#include "TM16xx.h"   // will include Arduino.h

#define TM1680_MAX_POS 32  // display modes 16x24, 8x32, 24x16, 32x8

#define TM1680_I2C_ADDRESS 0x73   // Regular Arduino I2C requires A0 to be low. Possible 7-bit addresses: 0x73 and 0x72 (0x73: A0 LOW, A1 HIGH or NC; 0x72: A0+A1 LOW)

// Display Commands (see Command Summary Table on page 17 of datasheet v1.1-EN)
#define TM1680_SYS_DIS 0x80    // * sytem disable
#define TM1680_COM_00 0xA0     //   00= 8-com NMOS -> 32SEG x 8COM  => max 32 digits on a 7-segment CA LED display (not tested yet)
#define TM1680_COM_01 0xA4     // * 01=16-com NMOS -> 24SEG x 16COM => max 24 digits on a 15-segment CA LED display (tested 5241BS dual digit, 14-seg + dot)
#define TM1680_COM_02 0xA8     //   10= 8-com PMOS -> 32COM x 8SEG 
#define TM1680_COM_03 0xAC     //   11=16-com PMOS -> 24COM x 16SEG 
#define TM1680_RC_0 0x98       //   RC Master Mode 0 (OSC low, SYN high, single chip)
#define TM1680_RC_1 0x9A       // * RC Master Mode 1 (cascade chip, see datasheet slave mode 0x90, EXT modes 0x9C/0x9E)
#define TM1680_SYS_EN 0x81     // * sytem enable
#define TM1680_LED_OFF 0x82    //   LED off
#define TM1680_LED_ON 0x83     // * LED on
#define TM1680_PWM_DUTY_0 0xB0 // * PWM duty (B0=min, BF=max)
#define TM1680_BLINK_OFF 0x88  // * Blink off
#define TM1680_BLINK_2HZ 0x89  //   Blink 2Hz
#define TM1680_BLINK_1HZ 0x8A  //   Blink 1Hz
#define TM1680_BLINK_05HZ 0x8B //   Blink 0.5Hz


// Display address command: 0xxx xxxx
// addresses 0x00 - 0x5F = 0-95 in 24x16 mode, 0x00 - 0x3F = 0-63 in 32x8 mode
// Two bytes per 16 com-digit: First digit: seg0-7: 0x00, seg 8-15: 0x01
#define TM1680_CMD_ADDRESS           0x00

// modes for setting interrupt pin and reading buttons are not supported (yet)

class TM1680 : public TM16xx
{
  public:
    /** Instantiate a TM1680 module specifying the data pin, number of digits */
    /** DEPRECATED: activation, intensity (0-7) and display mode are no longer used by constructor. */
    TM1680(byte i2cAddress, byte sdaPin, byte sclPin, byte numDigits);
    TM1680(byte i2cAddress, byte numDigits) : TM1680(i2cAddress, 255, 255, numDigits) {};
    TM1680(byte i2cAddress) : TM1680(i2cAddress, 255, 255, 4) {};
    // TODO: remove deprecated parameters - TM1680(byte i2cAddress, byte sdaPin, byte sclPin, byte numDigits=4);
    virtual void clearDisplay();
    virtual void setupDisplay(bool active, byte intensity=7);
    virtual void begin(bool activateDisplay=true, byte intensity=7);

	  virtual void setSegments(byte segments, byte position);   // will duplicate G to G1/G2 in 15-segment
	  virtual void setSegments16(uint16_t segments, byte position);   // some modules support more than 8 segments
    /** Set an Ascii character on a specific location (overloaded for 15-segment display) */
		virtual void sendAsciiChar(byte pos, char c, bool dot, const byte font[] = TM16XX_FONT_DEFAULT); // public method to allow calling from TM16xxDisplay


	  /** Set PROGMEM mapping array to be used when displaying segments */
    virtual void setSegmentMap(const byte *pMap);

	  // Set mapping array to be used when displaying segments
	  // The array should contain _maxSegments bytes specifying the desired mapping
	  //virtual void setSegmentMap(const byte aMap[]);
	  virtual void setAlphaNumeric(bool fAlpha, const byte *pMap=NULL);  // PGM_P OK???? on ESP32 core 3.0.2, tinyPico, works for ESP8266 (PGM_P defined as const char * in pgmspace.h)

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
    bool fAlphaNumeric=true;     // If true use 14-segment alphanumeric  display when having at least 13 SEG (15-seg: 14-segments plus dot)
    uint8_t _i2cAddress;
		//const PROGMEM byte *_pSegmentMap=NULL;               // pointer to segment map set using setSegmentMap()
		//PGM_P _pSegmentMap=NULL;               // pointer to segment map set using setSegmentMap() - PGM_P needed in ESP8266 core 3.0.2
    const byte *_pSegmentMap=NULL;               // pointer to segment map set using setSegmentMap() - PGM_P needed in ESP8266 core 3.0.2
};

// For alternative segment wiring, use setSegmentMap(). Example:   module.setSegmentMap(TM1680_SEGMAP_QYF0231);
//const PROGMEM byte TM1680_SEGMAP_QYF0231[]={0, 1, 2, 3, 4, 5, 6, 14, 7, 8, 9, 10, 13, 12, 11, 15};      // mapping for QYF-0231 HT16K33 module with dual 5241AS 14-segment + DP
const byte TM1680_SEGMAP_QYF0231[] PROGMEM ={0, 1, 2, 3, 4, 5, 6, 14, 7, 8, 9, 10, 13, 12, 11, 15};      // mapping for QYF-0231 HT16K33 module with dual 5241AS 14-segment + DP

const byte TM1680_SEGMAP_14SEG_EFG[] PROGMEM = {4, 5, 6, 7, 0, 1, 2, 3, 12, 13, 14, 15, 8, 9, 10, 11};      // mapping for TM1680 segment order EFGP ABCD LMNx gHJK on 14-segment plus dot LED display.


#endif
