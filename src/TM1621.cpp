/*
TM1621 - Library implementation for TM1621/TM1621D/TM1622 based on generic TM16xxIC.

Driver class for TM1621/TM1621D/TM1622 based on the generic TM16xxIC class.
It offers simple instantiation, similar to regular TM16xx classes.

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment displays and use Adafruit GFX on matrix displays.
*/

#include "TM1621.h"

TM1621::TM1621(if_ctrl_tm16xx ctrl, byte dataPin, byte wrPin, byte csPin, byte numDigits, byte layoutLCD)
  : TM16xxIC(ctrl, dataPin, wrPin, csPin, numDigits)  //  wrPin=clockPin, csPin=strobePin
{ // construtor for TM1621
  // TM1621 has DATA/_WR/_RD/_CS, TM162D has no _RD, so only DATA/_WR/_CS, that behave like DIN/CLK/STB
  this->_layout=layoutLCD;
  if(_ctrl.if_type==TM16XX_IFTYPE_4WSERB)
  {
    this->dataPin=dataPin;
    digitalWrite(this->dataPin, HIGH);
  }
}

void TM1621::stop(void)
{ // Stop sequence: stb^, wait, dat^
  digitalWrite(this->strobePin, HIGH);
  bitDelay();
  digitalWrite(this->dataPin, HIGH);      // Reset data
}

void TM1621::start(void)
{ // start by setting CS low
  digitalWrite(this->strobePin, LOW);      // Start  sequence
  bitDelay();
}

void TM1621::sendBits(uint16_t uFullData, byte nNumBits)
{ // send 12/13-bit data that includes the command mode: 100 for command, 101 for data
  for (byte i = 0; i < nNumBits; i++) {
    digitalWrite(this->clockPin, LOW);    // Start write sequence
    if (uFullData & 0x8000) {
      digitalWrite(this->dataPin, 1);  // Set data
    } else {
      digitalWrite(this->dataPin, 0);  // Set data
    }
    bitDelay();
    digitalWrite(this->clockPin, HIGH);    // Write data
    bitDelay();
    uFullData <<= 1;
  }
}

void TM1621::send(byte data)
{ // instead of regular 8-bit send(), TM1621 expects 12 bits for commands or 13 bits for data
}

void TM1621::sendCommand(byte cmd)
{ // send the command: 0b100cccccccc00000
  uint16_t uFullData=(0x0400 | cmd) << 5;  // cmd: 8-bit value + 0
	start();
  sendBits(uFullData, 12);
  stop();
}

void TM1621::sendData(byte address, byte data)
{ // send the data using fixed addressing mode: 0b101aaaaaadddd0 (6-bit address a5-a0, 4-bit data d0-d3)
  uint16_t uFullData=((0x0500 | (address<<2)) << 5) | ((data&0x0f)<<3);  // address+data
  begin();    // begin() is implicitly called upon first sending of display data, but executes only once.
	start();
  sendBits(uFullData, 13);
  stop();
}

void TM1621::setupDisplay(bool active, byte intensity)
{  // switch display on
  if(active)
  {
    sendCommand(TM1621_SYS_EN);
    sendCommand(TM1621_LCD_ON);
    // depending on the voltage 1/2 or 1/3 bias drive may be preferable. Use intensity 2 or 3 to specify which bias is used.
    // To display numbers all 4 COM lines of TM1621 are used in most configurations.
    // In the TM1621 datasheet this is specified in the figure in section "Pin drive waveform" (TM1621 v1.1, p13)
    // The TM1622 has COM0-COM7 and ignores the commmand to select 1/2/4 COM lines and to set the BIAS level. (Pins T1-T3 are for BIAS, but recommended not to be used)
    sendCommand(intensity<=2?TM1621_BIAS_12_4 :TM1621_BIAS_13_4);   // Bias 1/2 gives darker result than 1/3
  }
  else
  {
    sendCommand(TM1621_LCD_OFF);
    sendCommand(TM1621_SYS_DIS);
  }
}

void TM1621::clearDisplay(bool fOff)
{
    // Clear the display; TM1621 has 32 addresses for SEG0-SEG31; TM1621D has 23 addresses but only uses SEG9-SEG22;
    // TM1621B seems same as TM1621; TM1621C has 31 addresses but only uses SEG0-SEG5, SEG13-SEG19, SEG26-30;
    // TM1622 has a similar memory layout with 4-bits per address, with COM0-COM3 on even addresses and COM4-COM7 on odd addresses
    for(int i=0; i<_ctrl.grid_len; i++)
      sendData(i, fOff?0:0xFF);
}

void TM1621::setSegments16(uint16_t segments, byte position)
{ // NOTE: parameters are supplied as TM16xx segments/position
  //   uint16_t segments: segments A-G,P for 7-segment displays
  //   byte position: 0=leftmost digit
  //
  // TM1621/TM1621B have segments COM0-COM3 x SEG0-SEG31, 4-bit memory address 0-31
  // TM1621C has segments COM0-COM3 x SEG0-SEG5, SEG13-SEG19, SEG26-SEG30, with 4-bit memory 0-30;
  // TM1621D has segments COM0-COM3 x SEG9-SEG22, with 4-bit memory 0-22;
  // TM1622 has SEGMENTS C0M0-COM7 x SEG0-SEG31, with 4-bit memory addresses 0-63; QFP44 only has pins SEG0-SEG23.
  
/*
A suggested segment layout is shown in the datasheet:
  - TM1621 Datasheet v1.1 (chinese), Pin drive waveform figure 1 p.13
    TM1621C Datasheet v1.0 (chinese), Pin drive waveform figure 1 p.17
    TM1621D Datasheet v1.0 (chinese), Pin drive waveform figure 1 p.7; translated datasheet v1.0 figure 1 p.9
  
     |---A---|          Bit Segment      COM/SEG
     |       |          [0]       A     COM3/SEG1
     F       B          [1]       B     COM3/SEG0
     |       |          [2]       C     COM1/SEG0
     |---G---|          [3]       D     COM0/SEG0
     |       |          [4]       E     COM1/SEG1
     E       C          [5]       F     COM2/SEG1
     |       |          [6]       G     COM2/SEG0
 DP  |---D---|          [7]      DP     COM0/SEG1
  
Note that TM1621B datasheet v1.1 p.12 suggests an alternative layout.

Pinout 10-pin 3x7-segment LCD:

         A
       F   B
         G
       E   C
         D

   10  9  8  7  6 
    |  |  |  |  |
   +-------------+
   |             |\
   |  3   2   1  ||
   |             |/
   +-------------+
    |  |  |  |  |
    1  2  3  4  5

    pin seg
    com  1   2   3   4   5   6 
COM1 10     3D      2D      1D
COM2 9  3C  3E  2C  2E  1C  1E
COM3 8  3B  3G  2B  2G  1B  1G
COM4 7  3A  3F  2A  2F  1A  1F

Segment/Common order 6-digits 7-segment module
0: ABCP FGED 

Dots on seg 3 of positions 0, 2, 4
Bat+bar-1 on seg 3 of position 6
Bar-2 on seg 3 of position 8
Bar-3 on seg 3 of position 10
*/
  // Map segments if specified for alternative segment wiring.
  if(pSegmentMap)
    segments=mapSegments16(segments, pSegmentMap);

  // conversion of regular TM16xx segments/position to TM1621 LCD segments/common
  if(this->_layout==TM1621D_LAYOUT_4C2S_0CBA_DEGF)
  { // 7-segment LCD on TM1621D with first digit at SEG9/SEG10
    // 4-com/2-seg per digit; segment order: 0CBA DEGF
    byte segGFED=(segments & 0x78)>>3;  // segments GFED need to be reordered to DEGF
    byte segDEGF=((segGFED&0x1)<<3) | ((segGFED&0x2)<<1) | ((segGFED&0x4)>>2) | ((segGFED&0x8)>>2);  // shift GFED bits to their specific position
    sendData(9+(position<<1), segDEGF);                 // segments -GFED--- go to the first address
    sendData(9+((position<<1)|1), segments & 0x07);     // segments -----CBA go to the second address
  }
  else if(this->_layout==TM1621_LAYOUT_4C2S_ABCP_FGED_REV)
  { // 4-com/2-seg per digit; segment order: ABCP FGED, reversed digits (as used in LCD module PDC-6X1)
    byte segABCP=(segments & 0x07) | ((segments & 0x80)>>4);      // segments ABC + DP
    byte segDEFG=((segments & 0x78)>>3);                          // segments DEFG
    byte segFGED=((segDEFG&0x1)<<3) | ((segDEFG&0x2)<<1) | ((segDEFG&0x4)>>2) | ((segDEFG&0x8)>>2);
    position=this->digits-position-1;     // reverse the digit order
    sendData((position<<1), segABCP);     // segments ABCP--- on first address
    sendData(((position<<1)|1), segFGED); // segments 0----CBA on second address
  }
  else
  { // assume 4-COM, 2-SEG layout by default: TM1621_LAYOUT_4C2S_PGFE_DCBA
    sendData((position<<1), segments & 0x0F);     // SEGx+1: segments ABCD on first address (S0)
    sendData(((position<<1)|1), (segments&0xF0)>>4);      // SEGx+0: segments EFGP on second address (S1)
  }
}
