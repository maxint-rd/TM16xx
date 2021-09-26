/*

TM16xxMatrix.h - LED Matrix library for TM16xx, max 16 segments.

Made by Maxint R&D. See https://github.com/maxint-rd/

*/
#include "TM16xxMatrix16.h"

TM16xxMatrix16::TM16xxMatrix16(TM16xx *pTM16xx, byte nColumns, byte nRows)
{
	_pTM16xx=pTM16xx;
	_nColumns=nColumns;
	_nRows=nRows;
	
	// offscreen bitmap is required to set an individual pixel, while retaining the others 
	// TODO: use dynamic memory allocation for the off-screen bitmap
	// as different chips support different sizes
}

void TM16xxMatrix16::setColumn(byte nCol, uint16_t uPixels, bool fRender)
{
	_uColumns[nCol]=uPixels;
	if(fRender)
  	_pTM16xx->setSegments16(uPixels, nCol);
}

void TM16xxMatrix16::setAll(bool fOn, bool fRender)
{
	for(byte nCol=0; nCol<_nColumns; nCol++)
		setColumn(nCol, fOn?0xFFFF:0, fRender);
}

void TM16xxMatrix16::setPixel(byte nCol, byte nRow, bool fOn, bool fRender)
{
	uint16_t uColumn=_uColumns[nCol];
	if(fOn)
		uColumn=uColumn | _BV(nRow);
	else
		uColumn=uColumn & ~_BV(nRow);
	setColumn(nCol, uColumn, fRender);
}

void TM16xxMatrix16::render(void)
{ // render the offscreen bitmap to display all pixels
	for(byte nCol=0; nCol<_nColumns; nCol++)
  	_pTM16xx->setSegments16(_uColumns[nCol], nCol);
}
	
bool TM16xxMatrix16::getPixel(byte nCol, byte nRow)
{
	return((_uColumns[nCol]&bit(nRow))!=0);
}
