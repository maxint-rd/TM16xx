/*
TM1638Anode.cpp - Library implementation for TM1638 with Common Anode up to 10 digits.

Made by Maxint R&D, based on TM1638 class. See https://github.com/maxint-rd/
Modified by Simon Kong Win Chang (SkullKill) https://github.com/skullkill
Schemathics of Anode connection available here https://github.com/SkullKill/ESP32-Wall-Clock-PCB and https://github.com/SkullKill/ESP32-Wall-Clock-PCB/wiki

*/

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM1638Anode.h"

TM1638Anode::TM1638Anode(byte dataPin, byte clockPin, byte strobePin, boolean activateDisplay, byte intensity)
	: TM16xx(dataPin, clockPin, strobePin, TM1638Anode_MAX_POS, 10, activateDisplay, intensity)
{
  _maxSegments=8;		// on the QYF-TM1638 modules the two extra segment lines are not used. The display uses common anode LEDs
    //_numDigits = numDigits;
	memset(this->bitmap, 0, TM1638Anode_MAX_POS);
	clearDisplay();
	setupDisplay(activateDisplay, intensity);
}

void TM1638Anode::setSegments(byte segments, byte position)
{	// set 10 leds on common grd as specified
    // TM1638 uses 10 segments in two bytes, similar to TM1668
    // for the digit displays both byte (containing seg1-seg10) are sent
	if(position<_maxDisplays)
	{
		//update our memory bitmap
		this->bitmap[position]=segments;

		// transpose the bitmap to counter Common Anode connections
		for(byte nPos=0; nPos < _maxSegments; nPos++)
		{
			uint16_t btVal = 0;
			for (byte j = 0; j < TM1638Anode_MAX_POS; j++)
			{
				// for schematics of inverted connection, 1st digit connected to SEG10, 2nd digit connected to SEG9 etc
				//btVal |= ((pVal[j] >> nPos) & 1) << (TM1638Anode_MAX_POS - j - 1);
				// for schematics of 1st digit connected to SEG1, 2nd digit connected to SEG2 etc
				btVal |= ((this->bitmap[j] >> nPos) & 1) << (j);
			}
			// send each byte of the transposed bitmap
			//send 1st byte of data
			sendData(nPos << 1, (byte)btVal&0xFF);
			// send 2nd byte of data
			sendData((nPos << 1) | 1, (byte)(btVal>>8)&0x03);
		}
		
	}
}



void TM1638Anode::clearDisplay()
{
  for(byte nPos=0; nPos<TM1638Anode_MAX_POS; nPos++)
  {
	  // all OFF
	  sendData(nPos << 1, 0);
	  sendData((nPos << 1) | 1, 0);
	  // all ON
	  //sendData(nPos << 1, 0b11111111);
	  //sendData((nPos << 1) | 1, 0b00000011);
  }
}

uint32_t TM1638Anode::getButtons(void)
{
	// TM1638 returns 4 bytes/8 nibbles for keyscan. Each byte has K3, K2 and K1 status in lower bits of each nibble for KS1-KS8
	// NOTE: K3 is implemented for this class, but the TM1638Anode module only uses K1/K2
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