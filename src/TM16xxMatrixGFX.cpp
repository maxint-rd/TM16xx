/*

TM16xxMatrixGFX.h - Adafruit GFX LED Matrix library for TM16xx.

Made by Maxint R&D. See https://github.com/maxint-rd/

*/
#include "TM16xxMatrixGFX.h"

#if has_Adafruit_GFX

#define TM16xxMatrixGFX_swap(a, b) { int16_t t = a; a = b; b = t; }

TM16xxMatrixGFX::TM16xxMatrixGFX(TM16xx *pModule, byte nColumns, byte nRows) : Adafruit_GFX(nRows, nColumns)
{
	_nColumns=nColumns;
	_nRows=nRows;
	_fMirrorX=false;
	_fMirrorY=false;
	
	// Allocate a module array for just one module
	_nModules=1;
	_aModules=(TM16xx **)malloc(sizeof(TM16xx *));
	_aModules[0]=pModule;

	// An offscreen bitmap is required to set an individual pixel, while retaining the others 
	// We use dynamic memory allocation for the off-screen bitmap
	// as different chips support different sizes
	TM16xxMatrixGFX::bitmapSize = _nColumns;
  TM16xxMatrixGFX::bitmap = (byte*)malloc(bitmapSize);
  fillScreen(0);
}

TM16xxMatrixGFX::TM16xxMatrixGFX(TM16xx *aModules[], byte nColumns, byte nRows, byte nModulesCol, byte nModulesRow) : Adafruit_GFX(nRows*nModulesRow, nColumns*nModulesCol)
{
	_nColumns=nColumns;
	_nRows=nRows;
	_fMirrorX=false;
	_fMirrorY=false;

	// Allocate memory to copy pointers to the modules
	_nModules=nModulesRow*nModulesCol;
	_aModules=(TM16xx **)malloc(_nModules*sizeof(TM16xx *));
	for(byte n=0; n<_nModules; n++)
			_aModules[n]=aModules[n];

	// Allocate memory for the memory bitmap
	_nModulesRow=nModulesRow;
	_nModulesCol=nModulesCol;
	TM16xxMatrixGFX::bitmapSize = _nColumns*nModulesRow*nModulesCol;	// assume 8 rows per byte
  TM16xxMatrixGFX::bitmap = (byte*)malloc(bitmapSize);
  fillScreen(0);
}

void TM16xxMatrixGFX::setIntensity(byte intensity)
{	// set the intensity of all modules
	for(byte n=0; n<_nModules; n++)
			_aModules[n]->setupDisplay(true, intensity);
}

void TM16xxMatrixGFX::setMirror(bool fMirrorX, bool fMirrorY)	// fMirrorX=false, fMirrorY=false
{
	_fMirrorX=fMirrorX;
	_fMirrorY=fMirrorY;
}


void TM16xxMatrixGFX::fillScreen(uint16_t color)
{	// set the offscreen bitmap to the specified color
  memset(bitmap, color ? 0xff : 0, bitmapSize);
}

bool TM16xxMatrixGFX::convertToMemPos(int16_t &x, int16_t &y)
{	// Convert x/y coordinates to bitmap memory position (array with rows of 8 pixels per byte)
	// Given coordinates are passed by reference and changed to the required range
	// Returns false if coordinates fall outside of canvas after processing rotation
	if (rotation)
	{
		// Implement Adafruit's rotation.
		byte tmp;
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
		return(false);
	}

/*		// TODO?: support for different module orientaton and layout? (currently only left-top to right-bottom)
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
*/

	// mirror display (fMirrorX true for WeMOS mini matrix)
	if(_fMirrorX)
		x=WIDTH-x-1;
	if(_fMirrorY)
		y=HEIGHT-y-1;

	// Translation for multiple modules.
	if(_nModules>1)
	{	// Assume modules are identical and ordered left to right, top to bottom
		// The columns are stacked in memory in module order
		uint8_t _nModule=x/_nRows + _nModulesRow*(y/_nColumns);
		y=y%_nColumns+(_nModule*_nColumns);
		x=x%_nRows;
	}
	return(true);
}

void TM16xxMatrixGFX::drawPixel(int16_t xx, int16_t yy, uint16_t color)
{	// set the specified pixel as wanted in the memory
	// Operating in bytes is faster and takes less code to run. We don't
	// need values above 200, so switch from 16 bit ints to 8 bit unsigned
	// ints (bytes).
	//int8_t x = xx;
	//int8_t  y = yy;

	int16_t x = xx;
	int16_t  y = yy;

	if(!convertToMemPos(x, y))
		return;

	if(color)
	{
		bitmap[y]|=(1<<x);
	}
	else
	{
		bitmap[y]&=~(1<<x);
	}
}

 // required for scroll support as implemented by Adafruit GFX pull request #60
uint16_t TM16xxMatrixGFX::getPixel(int16_t x, int16_t y)
{
  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
    return 0;

	if(!convertToMemPos(x, y))
		return 0;

  return (bitmap[y+ (x/8)*WIDTH] >> (x%8)) & 0x1;
}

void TM16xxMatrixGFX::write()
{	// write the memory to the display
	for(uint8_t n=0;n<_nModules;n++)
	{
		for(uint8_t i=0;i<_nColumns;i++)
		{
			_aModules[n]->setSegments(bitmap[i+(n*_nColumns)],i);
		}
	}
}
#endif  // #if has_Adafruit_GFX
