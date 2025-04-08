/*
TM1622.h - Library implementation for TM1622 based on TM1621 + TM16xxIC.

Driver class for TM1622 based on the TM1621 class and the generic TM16xxIC class.
It offers simple instantiation, similar to regular TM16xx classes.

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment displays and use Adafruit GFX on matrix displays.
*/

#ifndef TM1622_h
#define TM1622_h

// TM1622 is very similar to TM1621. While it offers 8 COM x 32 SEG max, its display memory has the same organisation as TM1621.
// The initialisation sequence can be identical, but the BIAS command is ignored.
#include "TM1621.h"   // will include TM16xx.h via TM16xxIC.h

// TM1622 has COM0-COM7, but addresses are organized as nibbles. COM0-COM3 are on even address, COM4-COM7 are on odd addresses.
#define IC_TM1622    if_ctrl_tm16xx {TM16XX_IFTYPE_4WSERB, 5, 64, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_NIB, 0x000F, 0, 0x00} // like TM1621, but 8 COM x 32 SEG max
#define IC_TM1622Q44 if_ctrl_tm16xx {TM16XX_IFTYPE_4WSERB, 5, 48, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_NIB, 0x000F, 0, 0x00} // TM1622_QFP44 has SEG1-SEG23

#define TM1622_LAYOUT_DEFAULT 0x00                            // no special mapping: order PGFE_DCBA
#define TM1622_LAYOUT_8C1S_PGFE_DCBA TM1622_LAYOUT_DEFAULT
#define TM1622_LAYOUT_DM8BA10 0x08                            // 10-digit 16-segment+dot alphanumeric LCD display. 

class TM1622 : public TM1621
{
  public:
    // NOTE: to ensure calling the overloaded constructor and not the TM16xx base constructor, we need to match parameters and cannot use defaults
    TM1622(if_ctrl_tm16xx ctrl, byte dataPin, byte wrPin, byte csPin, byte numDigits, byte layoutLCD) : TM1621(ctrl, dataPin, wrPin, csPin, numDigits, layoutLCD) {}
  	TM1622(byte dataPin, byte wrPin, byte csPin, byte numDigits, byte layoutLCD): TM1621(IC_TM1622, dataPin, wrPin, csPin, numDigits, layoutLCD) {}
  	TM1622(byte dataPin, byte wrPin, byte csPin): TM1622(dataPin, wrPin, csPin, 8, 0) {}

    virtual void setSegments16(uint16_t segments, byte position);
};

/*
Regular 14-segment LED - segment labels and bit-assignment:
   |---A---|          Bit Segment     Bit Segment
   |\  |  /|          [0]       A    [ 8]      G2
   F H J K B          [1]       B    [ 9]       H
   |  \|/  |          [2]       C    [10]       J
   |-G1-G2-|          [3]       D    [11]       K
   |  /|\  |          [4]       E    [12]       L
   E N M L C          [5]       F    [13]       M
   |/  |  \|          [6]      G1    [14]       N
   |---D---| DP       [7]      DP

DM8BA10 16-segment LCD - segment labels and bit-mapping:
   |-A1-A2-|          Bit Segment     Bit Segment
   |\  |  /|          [0]      D2    [ 8]      D1
   F H J K B          [1]       E    [ 9]       C
   |  \|/  |          [2]       M    [10]       L
   |-G1-G2-|          [3]       N    [11]      G2
   |  /|\  |          [4]      G1    [12]       K
   E N M L C          [5]       F    [13]       B
   |/  |  \|          [6]       H    [14]       J
   |-D2-D1-| DP       [7]      A1    [15]      A2

TM1622 QFP52 and LQFP64 have addresses 0-63. TM1622 QFP44 has only SEG0-SEG23 on address 0-47.
On the DM8BA10 the 10 digits use addresses 0-39.
One full digit uses 4 adresses: 0: bits 0-3; 1: bits 4-7; 2: bits 8-11, 3: bits 12-15.
The 9 dots in between the digits are located on SEG0/SEG1/SEG2 of addresses 41, 43 and 45; positions: 7,8,9 - 4,5,6 - 1,2,3. No dot after rightmost position (0).
*/

const byte SEGMAP_14SEG_DM8BA10[] PROGMEM = {15, 13, 9, 8, 1, 5, 4, 7, 11, 6, 14, 12, 10, 2, 3, 0};      // mapping for DM8BA10 LCD-display module (with dot on bit 7 to be changed later)

class TM1622_DM8BA10 : public TM1622
{ //  DM8BA10 alphanumeric LCD display module: 10 x 16-segment digits + 9 dots between the digits
  public:
    // NOTE: to ensure calling the overloaded constructor and not the TM16xx base constructor, we need to match parameters and cannot use defaults
  	TM1622_DM8BA10(byte dataPin, byte wrPin, byte csPin, byte numDigits, byte layoutLCD): TM1622(IC_TM1622Q44, dataPin, wrPin, csPin, numDigits, layoutLCD) { setAlphaNumeric(true, SEGMAP_14SEG_DM8BA10); }
  	TM1622_DM8BA10(byte dataPin, byte wrPin, byte csPin): TM1622_DM8BA10(dataPin, wrPin, csPin, 10, TM1622_LAYOUT_DM8BA10) {}

    virtual void setSegments16(uint16_t segments, byte position);
};

#endif // #ifndef TM1622_h
