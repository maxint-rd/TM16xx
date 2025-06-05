/*

TM16xxMatrixGFX.h - Adafruit GFX LED Matrix class for TM16xx.
The TM16xxMatrixGFX class supports LED matrices of various sizes to be connected
to a TM16xx chip, such as TM1640 or TM1638. 

These are the resolutions supported by the most popular TM16xx chips:
   TM1637  8x6  (common anode)
   TM1638  10x8  (common cathode)
   TM1640  8x16  (common cathode)
	 TM1668  10x7  (common cathode)   

The library supports modules with either 8x8 or 8x16 pixels 

Made by Maxint R&D. See https://github.com/maxint-rd/

*/
#ifndef _TM16XX_MATRIXGFX_H
#define _TM16XX_MATRIXGFX_H

#include "TM16xx.h"

#ifdef __has_include
    #if __has_include(<Adafruit_GFX.h>)
        // GCC supports __has_include, others may not...
        #include <Adafruit_GFX.h>
        #define has_Adafruit_GFX 1
    #else
        #warning "TM16xxMatrixGFX requires AdafruitGFX. Install AdafruitGFX library when using TM16xxMatrixGFX."
        #define has_Adafruit_GFX 0
    #endif
#else
    #warning "Note: TM16xxMatrixGFX requires AdafruitGFX library."
    #define has_Adafruit_GFX 1 // maybe, but we cannot check...
#endif


#define TM16XX_MATRIXGFX_MAXCOLUMNS 16

#if has_Adafruit_GFX
class TM16xxMatrixGFX : public Adafruit_GFX
{
 public:
	TM16xxMatrixGFX(TM16xx *pModule, byte nColumns, byte nRows);
	TM16xxMatrixGFX(TM16xx *aModules[], byte nColumns, byte nRows, byte nModulesCol, byte nModulesRow);  // module layout left-top to right-bottom
  void setIntensity(byte intensity);		// intensity 0-7, 0=off, 7=bright
  void setMirror(bool fMirrorX=false, bool fMirrorY=false);
  void fillScreen(uint16_t color);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  uint16_t getPixel(int16_t x, int16_t y); // required for scroll support as implemented by Adafruit GFX pull request #60

  void write();

 protected:
  byte _nModules;
  byte _nModulesCol;
  byte _nModulesRow;
  TM16xx **_aModules;

  byte _nColumns;
  byte _nRows;
  bool _fMirrorX;
  bool _fMirrorY;
  
  byte *bitmap;
  byte bitmapSize;
  
 private:
 	bool convertToMemPos(int16_t &x, int16_t &y);
};
#endif  // #if has_Adafruit_GFX
#endif
