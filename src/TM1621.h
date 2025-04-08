/*
TM1621.h - Library implementation for TM1621 based on generic TM16xxIC.

Driver class for TM1621/TM1621D/TM1622 based on the generic TM16xxIC class.
It offers simple instantiation, similar to regular TM16xx classes.

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment displays and use Adafruit GFX on matrix displays.

References:
 - Tasmota driver for Sonoff POWR316D/POWR320D/THR316D/THR320D by Theo Arends (@arendst)
   https://github.com/Tasmota/tasmota/tasmota_xdrv_driver/xdrv_87_esp32_sonoff_tm1621.ino
   https://github.com/arendst/Tasmota/blob/d9d9ca2651363ac364f720403657e557e8a0efe8/tasmota/tasmota_xdrv_driver/xdrv_87_esp32_sonoff_tm1621.ino
*/

#ifndef TM1621_h
#define TM1621_h

#include "TM16xxIC.h"   // will include TM16xx.h

#define IC_TM1621   if_ctrl_tm16xx {TM16XX_IFTYPE_4WSERB, 5, 32, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_NIB, 0x000F, 0, 0x00}  // like TM1680, but 4-wire, RD not used
#define IC_TM1621B  if_ctrl_tm16xx {TM16XX_IFTYPE_4WSERB, 5, 32, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_NIB, 0x000F, 0, 0x00}  // same as TM1621
#define IC_TM1621C  if_ctrl_tm16xx {TM16XX_IFTYPE_4WSERB, 5, 31, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_NIB, 0x000F, 0, 0x00}  // like TM1621, only SEG0-SEG5, SEG13-SEG19, SEG26-30
#define IC_TM1621D  if_ctrl_tm16xx {TM16XX_IFTYPE_4WSERB, 5, 23, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_NIB, 0x000F, 0, 0x00}  // like TM1621, only SEG9-SEG22

//#define TM1621_PULSE_WIDTH   10    // microseconds (Sonoff = 100)

// TM1621 COMMANDS
#define TM1621_SYS_DIS       0x00  // 0b00000000
#define TM1621_SYS_EN        0x01  // 0b00000001
#define TM1621_LCD_OFF       0x02  // 0b00000010
#define TM1621_LCD_ON        0x03  // 0b00000011
#define TM1621_BIAS          0x29  // 0b00101001 = LCD 1/3 bias 4 commons option

#define TM1621_XTAL_32K      0x14  // 0b00010100
#define TM1621_RC_256K       0x18  // 0b00011000

// TM1621 BIAS OPTIONS (IGNORED ON TM1622)
#define TM1621_BIAS_12_2     0x20  // 0b001000X0   1/2 bias, 0b0010abX0 ab=00 for 2 common
#define TM1621_BIAS_12_3     0x24  // 0b001001X0   1/2 bias, 0b0010abX0 ab=01 for 3 common
#define TM1621_BIAS_12_4     0x28  // 0b001010X0   1/2 bias, 0b0010abX0 ab=10 for 4 common
#define TM1621_BIAS_13_2     0x21  // 0b001000X1   1/3 bias, 0b0010abX1 ab=00 for 2 common
#define TM1621_BIAS_13_3     0x25  // 0b001001X1   1/3 bias, 0b0010abX1 ab=01 for 3 common
#define TM1621_BIAS_13_4     0x29  // 0b001010X1   1/3 bias, 0b0010abX1 ab=10 for 4 common

// TIMER/BUZZER COMMANDS NOT AVAILABLE ON TM1621D
#define TM1621_TIMER_DIS     0x04  // 0b00000100,  0b00000110 to enable
#define TM1621_WDT_DIS       0x05  // 0b00000101,  0b00000111 to enable
#define TM1621_TONE_OFF      0x08  // 0b00001000,  0b00001001 for on
#define TM1621_IRQ_DIS       0x80  // 0b100x0xxx,  0b100x1xxx to enable

// LCD segment/digit manipulation, depending on LCD segment wiring
#define TM1621_LAYOUT_DEFAULT 0x00                            // no special mapping: order PGFE_DCBA
#define TM1621_LAYOUT_4C2S_PGFE_DCBA TM1621_LAYOUT_DEFAULT    // no special mapping: order PGFE_DCBA
#define TM1621_LAYOUT_4C2S_ABCP_FGED_REV 0x01                 // Address SEG9-SEG22; 4-com/2-seg per digit, 7-segments, order S1:CBA, S0:DEGF
#define TM1621_LAYOUT_PDC6X1 TM1621_LAYOUT_4C2S_ABCP_FGED_REV // 6-digit 7-segment display with dots on positions 4,5,6 and 3 level segments controlled by dots 1, 2 and 3
#define TM1621D_LAYOUT_4C2S_0CBA_DEGF 0x04                    // 4-com/2-seg per digit, 7-segments, order S1:CBA, S0:DEGF

// TM1621 initialization sequence
// full feature initialization: TM1621_SYS_EN, TM1621_LCD_ON, TM1621_BIAS, TM1621_TIMER_DIS, TM1621_WDT_DIS, TM1621_TONE_OFF, TM1621_IRQ_DIS
// minimal initialization: TM1621_SYS_EN, TM1621_LCD_ON, TM1621_BIAS

class TM1621 : public TM16xxIC
{
  public:
    // NOTE: to ensure calling the overloaded constructor and not the TM16xx base constructor, we need to match parameters and cannot use defaults
  	TM1621(if_ctrl_tm16xx ctrl, byte dataPin, byte wrPin, byte csPin, byte numDigits, byte layoutLCD);
  	TM1621(byte dataPin, byte wrPin, byte csPin, byte numDigits, byte layoutLCD): TM1621(IC_TM1621, dataPin, wrPin, csPin, numDigits, layoutLCD) {}
  	TM1621(byte dataPin, byte wrPin, byte csPin): TM1621(dataPin, wrPin, csPin, 4, 0) {}

   /** Set the LCD display active or inactive. Intensity level 2 or 3 can be used for selecting bias 1/2 or 1/3.
   Note that usually a potmeter is connected to the VLCD pin to tune the contrast level. */
    void setupDisplay(bool active, byte intensity=2);

    /** Clear the display */
    virtual void clearDisplay(bool fOff=true);

    /** Set the segments at a specific position on or off */
    //virtual void setSegments(byte segments, byte position);
    virtual void setSegments16(uint16_t segments, byte position);

  protected:
    virtual void start();
    virtual void stop();
    virtual void send(byte data);
    virtual void sendCommand(byte cmd);
    virtual void sendData(byte address, byte data);
    byte _layout=0;      // display layout of LCD segments/digits
    uint16_t _uDots=0;   // some memory to remember the setting of dots (modules may have them on separate addresses, eg. DM8BA10 10x16-segment LCD module)

  private:
    void sendBits(uint16_t uFullData, byte nNumBits);
};

class TM1621D : public TM1621
{
  public:
    // NOTE: to ensure calling the overloaded constructor and not the TM16xx base constructor, we need to match parameters and cannot use defaults
  	TM1621D(byte dataPin, byte wrPin, byte csPin, byte numDigits, byte layoutLCD): TM1621(IC_TM1621D, dataPin, wrPin, csPin, numDigits, layoutLCD) {}
  	TM1621D(byte dataPin, byte wrPin, byte csPin): TM1621D(dataPin, wrPin, csPin, 4, 0) {}
};

class TM1621_PDC6X1 : public TM1621
{ // PDC6X1 LCD display module: 6 x 7-segment digits + dots after last 3 digits and battery indicator instead of first three dots
  // The black blob on the back contains an HT1621 chip, which is compatible with TM1621.
  public:
    // NOTE: to ensure calling the overloaded constructor and not the TM16xx base constructor, we need to match parameters and cannot use defaults
  	TM1621_PDC6X1(byte dataPin, byte wrPin, byte csPin, byte numDigits, byte layoutLCD): TM1621(IC_TM1621, dataPin, wrPin, csPin, numDigits, layoutLCD) {}
  	TM1621_PDC6X1(byte dataPin, byte wrPin, byte csPin): TM1621_PDC6X1(dataPin, wrPin, csPin, 6, TM1621_LAYOUT_PDC6X1) {}
};

#endif // #ifndef TM1621_h