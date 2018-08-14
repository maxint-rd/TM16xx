/*

TM16xxMatrixGFX.h - Adafruit GFX LED Matrix library for TM16xx.

Made by Maxint R&D. See https://github.com/maxint-rd/

*/
#include <Adafruit_GFX.h>
#include "TM16xxMatrixGFX.h"

TM16xxMatrixGFX::TM16xxMatrixGFX(TM16xx *pTM16xx, byte nColumns, byte nRows) : Adafruit_GFX(nRows, nColumns)
{
	_pTM16xx=pTM16xx;
	_nColumns=nColumns;
	_nRows=nRows;
	_fMirrorX=false;
	_fMirrorY=false;
	
	// offscreen bitmap is required to set an individual pixel, while retaining the others 
	// TODO: use dynamic memory allocation for the off-screen bitmap
	// as different chips support different sizes
	TM16xxMatrixGFX::bitmapSize = _nColumns;
  TM16xxMatrixGFX::bitmap = (byte*)malloc(bitmapSize);
  fillScreen(0);
}

void TM16xxMatrixGFX::setIntensity(byte intensity)
{
  _pTM16xx->setupDisplay(true, intensity);
}

void TM16xxMatrixGFX::setMirror(boolean fMirrorX, boolean fMirrorY)	// fMirrorX=false, fMirrorY=false
{
	_fMirrorX=fMirrorX;
	_fMirrorY=fMirrorY;
}


void TM16xxMatrixGFX::fillScreen(uint16_t color) {
  memset(bitmap, color ? 0xff : 0, bitmapSize);
}

void TM16xxMatrixGFX::drawPixel(int16_t xx, int16_t yy, uint16_t color)
{	// set the specified pixel as wanted in the memory
	// Operating in bytes is faster and takes less code to run. We don't
	// need values above 200, so switch from 16 bit ints to 8 bit unsigned
	// ints (bytes).
	int8_t x = xx;
	byte y = yy;
	byte tmp;

	if ( rotation ) {
		// Implement Adafruit's rotation.
		if ( rotation >= 2 ) {										// rotation == 2 || rotation == 3
			x = _width - 1 - x;
		}

		if ( rotation == 1 || rotation == 2 ) {		// rotation == 1 || rotation == 2
			y = _height - 1 - y;
		}

		if ( rotation & 1 ) {     								// rotation == 1 || rotation == 3
			tmp = x; x = y; y = tmp;
		}
	}

	if ( x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT ) {
		// Ignore pixels outside the canvas.
		return;
	}

#if 0		// TODO: support for multiple chained displays
	// Translate the x, y coordinate according to the layout of the
	// displays. They can be ordered and rotated (0, 90, 180, 270).

	byte display = matrixPosition[(x >> 3) + hDisplays * (y >> 3)];
	x &= 0b111;
	y &= 0b111;

	byte r = matrixRotation[display];
	if ( r >= 2 ) {										   // 180 or 270 degrees
		x = 7 - x;
	}
	if ( r == 1 || r == 2 ) {				     // 90 or 180 degrees
		y = 7 - y;
	}
	if ( r & 1 ) {     								   // 90 or 270 degrees
		tmp = x; x = y; y = tmp;
	}

	byte d = display / hDisplays;
	x += (display - d * hDisplays) << 3; // x += (display % hDisplays) * 8
	y += d << 3;												 // y += (display / hDisplays) * 8

	// Update the color bit in our bitmap buffer.

	byte *ptr = bitmap + x + WIDTH * (y >> 3);
	byte val = 1 << (y & 0b111);

	if ( color ) {
		*ptr |= val;
	}
	else {
		*ptr &= ~val;
	}
#endif

	// mirror display (fMirrorX true for WeMOS mini matrix)
	if(_fMirrorX)
		x=WIDTH-x-1;
	if(_fMirrorY)
		y=HEIGHT-y-1;

	if(color)
	{
		bitmap[y]|=(1<<x);
	}
	else
	{
		bitmap[y]&=~(1<<x);
	}
}


void TM16xxMatrixGFX::write()
{	// write the memory to the display
	for(uint8_t i=0;i<_nColumns;i++)
	{
		_pTM16xx->setSegments(bitmap[i],i);
	}
}