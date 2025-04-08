/*
TM16xxIC - Library for TM16xx led display drivers.
Generic driver library to support multiple variants by specfied chip characteristics.

This generic class supports most 2 and 3 wire TM16xx chips, but has a larger footprint than
the chip specific class. E.g. for TM1637 The overhead is 1144 bytes flash and 16 bytes RAM
(tested on CH32V003, optimized for size -Os without LTO)

Other than having a different constructor the API of the generic class is compatible with 
the chip-specific classes.

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules with chips like TM1638 and TM1640, as well as individual chips.
Simply use print() on 7-segment or 14-segment displays and use Adafruit GFX on matrix displays.

Made by Maxint R&D. See https://github.com/maxint-rd
*/

#ifndef TM16XXIC_h
#define TM16XXIC_h

#include "TM16xx.h"   // will include Arduino.h


#define NIBBLE_HIGH(x)   ((((byte)(x)))&0xF0)
#define NIBBLE_LOW(x)   (((byte)(x))&0x0F)

// Interface types
#define TM16XX_IFTYPE_2WSER 2   // 2 wire serial interface like TM1640: DIN/CLK
#define TM16XX_IFTYPE_3WSER 3   // 3 wire serial interface like TM1638: DIO/CLK/STB or like TM1616: DIN/CLK/STB
#define TM16XX_IFTYPE_2WACK 6   // 2 wire non-addressable interface with ACK like TM1637: DIO/CLK
/*TODO*/ #define TM16XX_IFTYPE_0WIRE 0    // 0 wire no interface, only useful for debugging
/*DONT*/ #define TM16XX_IFTYPE_1WSER 1    // 1 wire serial interface like TM1652: DIN, fixed bitrate 19200
/*TODO*/ #define TM16XX_IFTYPE_4WSER 4    // 4 wire serial interface like TM1629/TM1623/: DIN/DOU/CLK/STB. Datasheets mention DI shorted to DO becomes DIO => IFTYPE_3WSER, (TODO: test hardware SPI: MOSI/MISO/SCK/SS=COPI/CIPO/SCK/CS)
/*TODO*/ #define TM16XX_IFTYPE_5WSER 5    // 3 wire like TM1649: DIO/SCLK/STB; separate DATA/CLK or BIN0-BIN3 for touch
/*DONT*/ #define TM16XX_IFTYPE_2WACKB 7   // 2 wire non-addressable interface with ACK like TM1650: DAT/CLK, deviating commands/bitorder/etc
/*TODO*/ #define TM16XX_IFTYPE_2WI2C 8    // 2 wire I2C addressable interface like TM1680: SDA/SCK
/*TODO*/ #define TM16XX_IFTYPE_4WSERB 9   // 4 wire serial interface like TM1681: DA/WR/RD/CS 

// Addressing mode; Number of bytes per GRD used for segment state
#define TM16XX_IC_SEGBYTES_1   1  // eg. TM1640: only one byte per grid for all segments
#define TM16XX_IC_SEGBYTES_2   2  // eg. TM1618: two bytes per grid  for all segments
#define TM16XX_IC_SEGBYTES_1SK 3  // eg. TM1650: one byte per grid, but skip odd addresses
/*TODO*/ #define TM16XX_IC_SEGBYTES_NIB 4  // eg. TM1680: one nibble per address

// multiplexed GRID/SEG lines (specifies how many of the gridlines can be assigned as SEG using TM16XX_CMD_MODE_xxGRID)
#define TM16XX_IC_SEGMUX_0    0   // 0 SEG/GRD pins
#define TM16XX_IC_SEGMUX_1 0x10   // 1 SEG/GRD pin
#define TM16XX_IC_SEGMUX_2 0x20   // 2 SEG/GRD pins
#define TM16XX_IC_SEGMUX_3 0x30   // 3 SEG/GRD pins
#define TM16XX_IC_SEGMUX_8 0x80   // 8 SEG/GRD pins

// Key decoding methods
#define TM16XX_KD_PRESS_HI 0x00      // eg. TM1638: when key is pressed the corresponding bit is high
/*TODO*/ #define TM16XX_KD_PRESS_LO 0x10      // TM1636: when key is pressed the corresponding bit is low
/*TODO*/ #define TM16XX_KD_PRESS_NN 0x20      // when key is pressed its number is received as normal number (0x06 = key 6)
#define TM16XX_KD_PRESS_NI 0x30      // eg. TM1651: when key is pressed its number is received as inversed number 0-n + bit 4 
                                     //    0xFF = no keys, 0xF7 = key0, 0xF1=key6, 0xF2=key5

// Interface control structure. Define how should we use the various chips
struct if_ctrl_tm16xx
{
  uint8_t if_type;    // Interface type - see list above, not all implemented yet
  uint8_t bit_delay;  // Delay between bits. Most TMxx chips have maximum of 1us plus 1us extra after the key read command
  uint8_t grid_len;   // Length of the chip's display memory (number of GRID positions)
  uint8_t seg_bytes;  // Width of the segment memory (usually 1 or 2 bytes per GRID) combined with the number of SEG pins that can be used as GRD
  uint16_t seg_mask;  // Mask specifying which segment bits are actually used (e.g. 0x03FF for the lower 10 bits)
  uint8_t key_scan;   // Number of bytes needed to read the state of the buttons combined with decoding method
  uint8_t key_mask;   // Mask specifying which key bits are artually used (0x77 for the lower 3 bits of each nibble, 0xEE for the higher 3 bits)
};

//
// Tested chips. These definitions were tested to work in at least one setup
//
#define IC_TM1616  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 1, 4, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_1SK, 0x007F, 0, 0x00}  // SxG: 7x4 CC
#define IC_TM1618  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 7, TM16XX_IC_SEGMUX_3 | TM16XX_IC_SEGBYTES_2, 0x381F, 3, 0x12}  // SxG:8x4-5x7 CC/CA, K:5x1
#define IC_TM1623  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 7, TM16XX_IC_SEGMUX_3 | TM16XX_IC_SEGBYTES_2, 0x3FFF, 5, 0x3F}  // SxG: 14x4-11x7 CC/CA, K:10x3, DIN/DOU/CLK/STB dataheet v2.4 p13 has DI connected to DO.
#define IC_TM1624  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 7, TM16XX_IC_SEGMUX_3 | TM16XX_IC_SEGBYTES_2, 0x3FFF, 0, 0x00}  // SxG: 14x4-11x7 CC/CA
#define IC_TM1628  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 7, TM16XX_IC_SEGMUX_3 | TM16XX_IC_SEGBYTES_2, 0x3BFF, 5, 0x1B}  // SxG: 13x4-10x7 CC/CA, K:10x2, 
#define IC_TM1637  if_ctrl_tm16xx {TM16XX_IFTYPE_2WACK, 5, 6, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_1, 0x00FF, TM16XX_KD_PRESS_NI | 1, 0x1F}  // SxG: 8x6 CA, K:8x2 single pressed key number 0-6 + B3/B4 inverted 
#define IC_TM1638  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 1, 8, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_2, 0x03FF, 4, 0x77}  // SxG:10x8 CC/CA, K:8x3
#define IC_TM1640  if_ctrl_tm16xx {TM16XX_IFTYPE_2WSER, 1, 16,TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_1, 0x00FF, 0, 0x00}  // SxG: 8x16 CC/CA
#define IC_TM1668  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 7, TM16XX_IC_SEGMUX_3 | TM16XX_IC_SEGBYTES_2, 0x3BFF, 5, 0x1B}  // SxG: 13x4-10x7 CC/CA, K:10x2; same as TM1628A in smaller package
/* TODO for TM1623: Test separate DI/DO with hardware SPI,  TM16XX_IFTYPE_4WSER */

//
// UNTESTED chips, only based on datasheet information. CHECK BEFORE USE.
//
#define IC_TA6932  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 16,TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_1, 0x00FF, 0, 0x00} // SxG: 8x16 CC
#define IC_TM1617  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 7, TM16XX_IC_SEGMUX_1 | TM16XX_IC_SEGBYTES_2, 0x0FF0, 5, 0x09}  // SxG:8x2-7x3 CC, K:6x1, chinese datasheet LCSC, address is 14 bytes from 00H to 0DH, only use: GRID1, GRID2, GRID7, SEG5-SEG12
#define IC_TM1620  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 6, TM16XX_IC_SEGMUX_2 | TM16XX_IC_SEGBYTES_2, 0x30FF, 0, 0x00}  // SxG: 10x4-8x6 CC
#define IC_TM1620B if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 7, TM16XX_IC_SEGMUX_3 | TM16XX_IC_SEGBYTES_2, 0x383F, 3, 0x12}  // SxG: 9x4-6x7 CC/CA, K:6x1
#define IC_TM1623C if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 7, TM16XX_IC_SEGMUX_3 | TM16XX_IC_SEGBYTES_2, 0x3FFF, 5, 0x3F}  // SxG: 14x4-11x7 CC/CA, K:10x3, same as TM1623? TM16XX_IFTYPE_4WSER - DIN/DOU/CLK/STB Datasheet v1.1 p2: "If DIN and DOUT use the same level, you can short-circuit DIN to multiplex DIN/DOUT"
#define IC_TM1626A if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 7, TM16XX_IC_SEGMUX_3 | TM16XX_IC_SEGBYTES_2, 0x3BFF, 5, 0x09}  // SxG: 13x4-10x7 CC/CA, K:10x1, PWM controller for LED1-3, SW2/SW3 ports, see fig.28 on p12
#define IC_TM1626B if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 7, TM16XX_IC_SEGMUX_3 | TM16XX_IC_SEGBYTES_2, 0x3FFF, 5, 0x3F}  // SxG: 14x4-11x7 CC/CA, K:10x3, PWM & SW2/SW3 (see TM1626A); chinese datasheet, 
#define IC_TM1627  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 7, TM16XX_IC_SEGMUX_3 | TM16XX_IC_SEGBYTES_2, 0x3BFF, 5, 0x1B}  // SxG: 13x4-10x7 CC/CA, K:10x2; KEYINT interrupt pin
#define IC_TM1628A if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 7, TM16XX_IC_SEGMUX_3 | TM16XX_IC_SEGBYTES_2, 0x3BFF, 5, 0x1B}  // same as TM1628, but with internal pull-up/down resistors
#define IC_TM1629  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 8, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_2, 0xFFFF, 4, 0xFF}  // SxG: 16x8 CC/CA, K:8x4; TM16XX_IFTYPE_4WSER - DIN/DOU/CLK/STB datasheet v2.4 p2: "With DIN shorted for DIO to use"
#define IC_TM1629A if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 8, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_2, 0xFFFF, 0, 0x00}  // SxG: 16x8 CC/CA
#define IC_TM1629B if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 8, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_2, 0x3FFF, 4, 0xBB}  // SxG: 14x8 CC/CA, K:8x2
#define IC_TM1629C if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 8, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_2, 0x7FFF, 4, 0x88}  // SxG: 15x8 CC/CA, K:8x1
#define IC_TM1629D if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 8, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_2, 0x0FFF, 4, 0xFF}  // SxG: 12x8 CC/CA, K:8x4
#define IC_TM1630  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 5, TM16XX_IC_SEGMUX_1 | TM16XX_IC_SEGBYTES_2, 0x00FF, 4, 0xFF}  // SxG: 8x4-7x5, K:7x1; chinese datasheet
#define IC_TM1636  if_ctrl_tm16xx {TM16XX_IFTYPE_2WACK, 5, 4, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_1, 0x00FF, TM16XX_KD_PRESS_NI | 1, 0x1F}  // SxG: 8x4 CA, K:8x2 single pressed key number 0-6 + B3/B4 inverted 
#define IC_TM1639  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 8, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_2, 0x0F0F, 2, 0xCC}  // SxG: 8x8 CC/CA, K:4x2
#define IC_TM1640B if_ctrl_tm16xx {TM16XX_IFTYPE_2WSER, 5, 16,TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_1, 0x00FF, 0, 0x00}  // same as TM1640, different current limits
#define IC_TM1642  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 7, TM16XX_IC_SEGMUX_3 | TM16XX_IC_SEGBYTES_2, 0x3BFF, 4, 0x1B}  // SxG: 13x4-10x7, K:8x2
/*DONT*/ #define IC_TM1643  if_ctrl_tm16xx {TM16XX_IFTYPE_2WSER, 5, 16,TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_1, 0x00FF, 0, 0x00}  // 16x8 point led, uncommon wiring
/*DONT*/ #define IC_TM1646  if_ctrl_tm16xx {TM16XX_IFTYPE_2WSER, 5, 18,TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_1, 0x00FF, 0, 0x00}  // 18x8 led, uncommon wiring like TM1643
/*TODO*/ #define IC_TM1648A if_ctrl_tm16xx {TM16XX_IFTYPE_2WI2C, 5, 7, TM16XX_IC_SEGMUX_3 | TM16XX_IC_SEGBYTES_2, 0x00FF, 1, 0xFF}  // SxG: 13x4-10x7 CC/CA, K:8x1 touch, i2C @50, 1x buz
/*TODO*/ #define IC_TM1649  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 8, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_2, 0x7FFF, 4, 0x88}  // SxG: 15x8, K:8x1 + 8x1 touch, chinese datasheet LCSC
#define IC_TM1651  if_ctrl_tm16xx {TM16XX_IFTYPE_2WACK, 5, 4, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_1, 0x007F, TM16XX_KD_PRESS_NI | 1, 0x0F}  // SxG: 7x4 CA, K:7x1 single pressed key number 0-6 + B3 inverted
#define IC_TM1665  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 7, TM16XX_IC_SEGMUX_3 | TM16XX_IC_SEGBYTES_2, 0x00FF, 4, 0xFF}  // no datasheet
#define IC_TM1667  if_ctrl_tm16xx {TM16XX_IFTYPE_3WSER, 5, 7, TM16XX_IC_SEGMUX_3 | TM16XX_IC_SEGBYTES_2, 0x3BFF, 5, 0x1B}  // SxG: 13x4-10x7 CC/CA, K:10x2, chinese datasheet
/*TODO*/ #define IC_TM1680  if_ctrl_tm16xx {TM16XX_IFTYPE_2WI2C, 5, 96,TM16XX_IC_SEGMUX_8 | TM16XX_IC_SEGBYTES_NIB, 0x0F0F, 0, 0x00}  // SxG: 32x8/24x16; A0 must be wired low for 7-bit I2C address 0x73, see issue #2, 16 level duty
/*TODO*/ #define IC_TM1681  if_ctrl_tm16xx {TM16XX_IFTYPE_4WSERB, 5, 96,TM16XX_IC_SEGMUX_8 | TM16XX_IC_SEGBYTES_NIB, 0x0F0F, 0, 0x00}  // like TM1680, but 4-wire serial interface (DA/WR/RD/CS)

//
// Chips only supported by chip specific class, when used in TM16xxIC these trigger a compiler error
//
//#define IC_TM1650  if_ctrl_tm16xx {TM16XX_IFTYPE_2WACKB, 5, 4, TM16XX_IC_SEGMUX_0 | TM16XX_IC_SEGBYTES_1SK, 0x00FF, TM16XX_KD_PRESS_NN | 1, 0xFF}  // SxG: 8x4-7x4 CC,  K:7x4, odd addresses skipped
#define IC_TM1650  (ERROR(" -- please include TM1650.H for TM1650 class -- "))     // TM1650 uses different protocol, commands, brightness level values and key reading

//#define IC_TM1652  if_ctrl_tm16xx {TM16XX_IFTYPE_1WSER, 5, 6, TM16XX_IC_SEGMUX_1 | TM16XX_IC_SEGBYTES_1, 0x00FF, 0, 0x00}  // SxG: 7x6-8x5 CC, 1-Wire serial, fixed 19200 Baud UART, 16 level duty, 8 level current
#define IC_TM1652  (ERROR(" -- please include TM1652.H for TM1652 class -- "))     // TM1652 uses different protocol, commands, brightness levels/drive current and key reading


// Regular segment order 7-segment display: 
//    01234567
//    ABCDEFGP
// Regular segment order 15-segment display: 
//    0123456789012345
//    ABCDEFGPgHJKLMNX (G=G1, g=G2, P=point, X=not used)

// Common Cathode segment maps
// For alternative 15-segment wiring, use setAlphaNumeric(). Example:   module.setAlphaNumeric(true, TM1628_SEGMAP_2X5241AS);
// pinout 1-19 for 19-pins dual 5241AS LED-display module: EN1MKGA2B4FH3JLgDPC (G=G1, g=G2, P=point)
// TM1628/TM1668 support 13x4 mode, requiring G1=G2, no DP.
const byte SEGMAP_13SEG_BB2X5241AS[] PROGMEM = {3, 2, 12, 10, 6, 1, 9, 15, 9, 0, 7, 4, 8, 5, 11, 15};      // mapping for LED-display dual 5241AS CC combined 19-pins, having G1=G2,no DP to allow for max 13 segments on breadboard
const byte SEGMAP_13SEG_2X5241AS[] PROGMEM =   {0, 1, 2, 3, 4, 5, 6, 15, 6, 7, 8, 9, 10, 11, 12, 15};      // mapping for LED-display dual 5241AS CC combined 19-pins, having G1=G2,no DP to allow for max 13 segments
const byte SEGMAP_14SEG_2X5241AS[] PROGMEM =   {0, 1, 2, 3, 4, 5, 6, 7, 6, 8, 9, 10, 11, 12, 13, 15};      // mapping for LED-display dual 5241AS CC having G1=G2, with DP to allow for max 14 segments

// Using define instead of PROGMEM variables makes no difference in flash/ram size on CH32V003
//#define SEGMAP_13SEG_BB2X5241AS (const byte[]){3, 2, 12, 10, 6, 1, 9, 15, 9, 0, 7, 4, 8, 5, 11, 15}      // mapping for LED-display dual 5241AS CC combined 19-pins, having G1=G2,no DP to allow for max 13 segments on breadboard
//#define SEGMAP_13SEG_2X5241AS (const byte[]){0, 1, 2, 3, 4, 5, 6, 15, 6, 7, 8, 9, 10, 11, 12, 15}      // mapping for LED-display dual 5241AS CC combined 19-pins, having G1=G2,no DP to allow for max 13 segments
//#define SEGMAP_14SEG_2X5241AS (const byte[]){0, 1, 2, 3, 4, 5, 6, 7, 6, 8, 9, 10, 11, 12, 13, 15}      // mapping for LED-display dual 5241AS CC having G1=G2, with DP to allow for max 14 segments

class TM16xxIC : public TM16xx
{
  public:
    /** Instantiate a TM16xxIC chip specifying interface, data/clock/strobe pins, the number of GRD-lines used */
    TM16xxIC(if_ctrl_tm16xx ctrl, byte dataPin, byte clockPin, byte strobePin, byte numGrdUsed=0);
    TM16xxIC(if_ctrl_tm16xx ctrl, byte dataPin, byte clockPin);    // constructor for chips without strobe

   /** Set the display (segments and LEDs) active or off and intensity (range from 0-7). */
   virtual void setupDisplay(bool active, byte intensity);   // For TM1618: also set the display mode (based on _maxSegments)

    /** Set the segments at a specific position on or off */
    virtual void setSegments(byte segments, byte position);
    virtual void setSegments16(uint16_t segments, byte position);   // some modules support more than 8 segments

    /** use alphanumeric display (yes/no) with or without segment map */  
    virtual void setAlphaNumeric(bool fAlpha=true, const byte *pMap=NULL);    // const byte aMap[]
    /** Set an Ascii character on a specific location (overloaded for 15-segment display) */
    virtual void sendAsciiChar(byte pos, char c, bool dot, const byte font[] = TM16XX_FONT_DEFAULT); // public method to allow calling from TM16xxDisplay

    /** Clear the display */
    virtual void clearDisplay();
 
    /** Returns the pressed buttons as a bit set (left to right). */
    virtual uint32_t getButtons();
  
  protected:
    virtual void bitDelay();
    //virtual void start();
    virtual void start();
    virtual void stop();
    virtual void send(byte data);
    uint16_t mapSegments16(uint16_t segments, const byte *pMap=NULL);
    uint16_t flipSegments16(uint16_t uSegments);
    if_ctrl_tm16xx _ctrl; // made protected to make it usable by derived classes
    const byte *pSegmentMap=NULL; // pointer to segment map for alphanumeric displays. set using setAlphaNumeric(); made protected to make it usable by derived classes

  private:
    uint8_t countMaxSEG(void);
    //if_ctrl_tm16xx _ctrl;
    bool fAlphaNumeric=false;     // If true use 15-segment alphanumeric  display when having at least 13 SEG (15-seg: 14-segments plus dot)
};

#endif