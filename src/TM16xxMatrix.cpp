/*

TM16xxMatrix.h - LED Matrix library for TM16xx.

Made by Maxint R&D. See https://github.com/maxint-rd/

*/
#include "TM16xxMatrix.h"

TM16xxMatrix::TM16xxMatrix(TM16xx *pTM16xx, byte nColumns, byte nRows)
{
	_pTM16xx=pTM16xx;
	_nColumns=nColumns;
	_nRows=nRows;
	
	// offscreen bitmap is required to set an individual pixel, while retaining the others 
	// TODO: use dynamic memory allocation for the off-screen bitmap
	// as different chips support different sizes
}

void TM16xxMatrix::setColumn(byte nCol, byte bPixels)
{
	_btColumns[nCol]=bPixels;
	_pTM16xx->setSegments(bPixels, nCol);
}

void TM16xxMatrix::setAll(bool fOn)
{
	for(byte nCol=0; nCol<_nColumns; nCol++)
		setColumn(nCol, fOn?0xFF:0);
}

void TM16xxMatrix::setPixel(byte nCol, byte nRow, bool fOn)
{
	byte btColumn=_btColumns[nCol];
	if(fOn)
		btColumn=btColumn | _BV(nRow);
	else
		btColumn=btColumn & ~_BV(nRow);
	setColumn(nCol, btColumn);
}
	
bool TM16xxMatrix::getPixel(byte nCol, byte nRow)
{
	return((_btColumns[nCol]&_BV(nRow))!=0);
}
