/*

TM16xxMatrix.h - LED Matrix class for TM16xx.
The TM16xxMatrix class supports LED matrices of various sizes to be connected
to a TM16xx chip, such as TM1640 or TM1638. 

These are the resolutions supported by the most popular TM16xx chips:
   TM1637  8x6  (common anode)
   TM1638  10x8  (common cathode)
   TM1640  8x16  (common cathode)
	 TM1668  10x7  (common cathode)   

Made by Maxint R&D. See https://github.com/maxint-rd/

*/
#ifndef _TM16XX_MATRIX_H
#define _TM16XX_MATRIX_H

#include "TM16xx.h"

#define TM16XX_MATRIX_MAXCOLUMNS 16

class TM16xxMatrix
{
 public:
	TM16xxMatrix(TM16xx *pTM16xx, byte nColumns, byte nRows);
	void setColumn(byte nCol, byte bPixels);
	void setAll(bool fOn);
	void setPixel(byte nCol, byte nRow, bool fOn);
	bool getPixel(byte nCol, byte nRow);
	inline byte getNumRows() { return(_nRows); }
	inline byte getNumColumns() { return(_nColumns); }

 protected:
  TM16xx *_pTM16xx;
  byte _nColumns;
  byte _nRows;
  
  byte _btColumns[TM16XX_MATRIX_MAXCOLUMNS]={0};
};
#endif
