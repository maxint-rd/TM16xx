/*
TM1638QYF.cpp - Library implementation for QYF-TM1638 module.
The QYF-TM1638 module uses the TM1638 chip with with a 2 xcommon anode 4bit 7-segment LED display.

Made by Maxint R&D, based on TM1638 class. See https://github.com/maxint-rd/
*/

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM1638QYF.h"

TM1638QYF::TM1638QYF(byte dataPin, byte clockPin, byte strobePin, bool activateDisplay, byte intensity)
	: TM16xx(dataPin, clockPin, strobePin, TM1638QYF_MAX_POS, 8, activateDisplay, intensity)
{
  _maxSegments=8;		// on the QYF-TM1638 modules the two extra segment lines are not used. The display uses common anode LEDs

	//memset(this->bitmap, 0, TM1638QYF_MAX_POS);
	clearDisplay();
	setupDisplay(activateDisplay, intensity);
}


/**
 * Flip a bitboard about the antidiagonal a8-h1.
 * Square a1 is mapped to h8 and vice versa.
 * @param x any bitboard
 * @return bitboard x flipped about antidiagonal a8-h1
 *
 * https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#Anti-Diagonal
 */
uint64_t TM1638QYF::flipDiagA8H1(uint64_t x)
{
	uint64_t t;
  const uint64_t k1 = 0xaa00aa00aa00aa00;
	const uint64_t k2 = 0xcccc0000cccc0000;
	const uint64_t k4 = 0xf0f0f0f00f0f0f0f;
	t  =       x ^ (x << 36) ;
	x ^= k4 & (t ^ (x >> 36));
	t  = k2 & (x ^ (x << 18));
	x ^=       t ^ (t >> 18) ;
	t  = k1 & (x ^ (x <<  9));
	x ^=       t ^ (t >>  9) ;

	return x;
}

/**
 * Flip a bitboard about the diagonal a1-h8.
 * Square h1 is mapped to a8 and vice versa.
 * @param x any bitboard
 * @return bitboard x flipped about diagonal a1-h8
 *
 * https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#Diagonal
 */
/*
uint64_t TM1638QYF::flipDiagA1H8(uint64_t x)
{
   uint64_t t;
   const uint64_t k1 = 0x5500550055005500;
   const uint64_t k2 = 0x3333000033330000;
   const uint64_t k4 = 0x0f0f0f0f00000000;
   t  = k4 & (x ^ (x << 28));
   x ^=       t ^ (t >> 28) ;
   t  = k2 & (x ^ (x << 14));
   x ^=       t ^ (t >> 14) ;
   t  = k1 & (x ^ (x <<  7));
   x ^=       t ^ (t >>  7) ;
   return x;
}
*/

void TM1638QYF::setSegments(byte segments, byte position)
{	// set 8 leds on common grd as specified
	// TODO: support 10-14 segments on chips like TM1638/TM1668
  // TM1638 uses 10 segments in two bytes, similar to TM1668
  // for the digit displays only the first byte (containing seg1-seg8) is sent
	// Only the LSB (SEG1-8) is sent to the display
	if(position<_maxDisplays)
	{
		//update our memory bitmap
		this->bitmap[position]=segments;

		// Transpose the bitmap to counter Common Anode connections
    // To avoid an alignment exception on the ESP8266 we cannot use the class-member as uint64_t directly,
    // but instead we use an intermediate buffer and memcpy. Note: this alignement issue didn't occur on the ESP32.
		byte buf[TM1638QYF_MAX_POS];
    memcpy(buf, this->bitmap, TM1638QYF_MAX_POS);
    *((uint64_t *)buf)=flipDiagA8H1(*((uint64_t *)buf));
		//*((uint64_t *)buf)=flipDiagA1H8(*((uint64_t *)buf);

		// send each byte of the transposed bitmap
		for(byte nPos=0; nPos<TM1638QYF_MAX_POS; nPos++)
		{
			sendData(((7-nPos) << 1), buf[nPos]);	// flip position to counter Common Anode connections
		}
	}
}

void TM1638QYF::clearDisplay()
{
	memset(this->bitmap, 0, TM1638QYF_MAX_POS);   // clear the memory bitmap
  for(byte nPos=0; nPos<TM1638QYF_MAX_POS; nPos++)
	  sendData(nPos << 1, 0);
}

uint32_t TM1638QYF::getButtons(void)
{
	// TM1638 returns 4 bytes/8 nibbles for keyscan. Each byte has K3, K2 and K1 status in lower bits of each nibble for KS1-KS8
	// NOTE: K3 is implemented for this class, but the TM1638QYF module only uses K1/K2
  byte keys_K1 = 0;
  byte keys_K2 = 0;
  byte keys_K3 = 0;

  start();
  send(TM16XX_CMD_DATA_READ); // B01000010 Read the key scan data

  for (int i = 0; i < 4; i++)
  {
	  byte rec = receive();
    keys_K1 |= ((rec&_BV(2))>>2 | (rec&_BV(6))>>5) << (2*i);			// bits 2 and	6 for K1/KS1 and K1/KS2
    keys_K2 |= ((rec&_BV(1))>>1 | (rec&_BV(5))>>4) << (2*i);			// bits 1 and	5 for K2/KS1 and K2/KS2
    keys_K3 |= ((rec&_BV(0))    | (rec&_BV(4))>>3) << (2*i);			// bits 0 and	4 for K3/KS1 and K3/KS2
  }

  stop();

  return((uint32_t)keys_K3<<16 | (uint32_t)keys_K2<<8 | (uint32_t)keys_K1);
}