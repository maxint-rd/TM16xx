/*

TM16xxMatrixGFX.h - Adafruit GFX LED Matrix class for TM16xx.
The TM16xxMatrixGFX class supports LED matrices of various sizes to be connected
to a TM16xx chip, such as TM1640 or TM1638. 
The

These are the resolutions supported by the most popular TM16xx chips:
   TM1637  8x6  (common anode)
   TM1638  10x8  (common cathode)
   TM1640  8x16  (common cathode)
	 TM1668  10x7  (common cathode)   

Made by Maxint R&D. See https://github.com/maxint-rd/

*/
#ifndef _TM16XX_MATRIXGFX_H
#define _TM16XX_MATRIXGFX_H

#include "TM16xx.h"

#define TM16XX_MATRIXGFX_MAXCOLUMNS 16

class TM16xxMatrixGFX : public Adafruit_GFX
{
 public:
	TM16xxMatrixGFX(TM16xx *pTM16xx, byte nColumns, byte nRows);
  void setIntensity(byte intensity);
  void fillScreen(uint16_t color);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void write();

 protected:
  TM16xx *_pTM16xx;
  byte _nColumns;
  byte _nRows;
  
  /* We keep track of the led-status for 8 devices in this array */
  byte *bitmap;
  byte bitmapSize;
};
#endif
