/*
TM1652.h - Library for TM1652 led display driver. Only DIN, 8x5/7x6 LED, no buttons.

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment displays and use Adafruit GFX on matrix displays.

Made by Maxint R&D. See https://github.com/maxint-rd
*/

#ifndef TM1652_h
#define TM1652_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM16xx.h"

#define TM1652_MAX_POS 6

// TM1652 has two display modes: 8 seg x 4 grd and 7 seg x 4 grd
#define TM1652_DISPMODE_5x8 0x00
#define TM1652_DISPMODE_6x7 0x01

#define TM1652_CMD_MODE  0x18
#define TM1652_CMD_ADDRESS  0x08

class TM1652 : public TM16xx
{
  public:
    /** Instantiate a TM1652 module specifying the data pin, number of digits */
    /** DEPRECATED: activation, intensity (0-7) and display mode are no longer used by constructor. */
    TM1652(byte dataPin, byte numDigits=4, bool activateDisplay=true, byte intensity=7, byte displaymode = TM1652_DISPMODE_5x8);
    // TODO: remove deprecated parameters - TM1652(byte dataPin, byte numDigits=4);
    virtual void clearDisplay();
    virtual void setupDisplay(bool active, byte intensity, byte driveCurrent = 6);
    virtual void begin(bool activateDisplay=true, byte intensity=7, byte driveCurrent = 6);

  protected:
    virtual void start();
    virtual void stop();
    virtual void send(byte data);
    virtual void sendData(byte address, byte data);
    virtual void sendCommand(byte cmd);

  private:
    byte reverseByte(byte b);
    void waitCmd(void);
    void endCmd(void);
    uint32_t tLastCmd;
};

#endif
