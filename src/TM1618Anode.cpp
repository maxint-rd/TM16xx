/*
TM1618Anode - Library implementation for TM1618 with Common Anode up to 5 digits of 7 segments.
TM1618: 5x7 - 8x4 SEGxGRD, 5 x 1 single button, DIO/CLK/STB

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment displays and use Adafruit GFX on matrix displays.

Made by Maxint R&D. See https://github.com/maxint-rd

TM1618 Pinout:
             +----v----+
        DIO -|1      18|- GRD1
        CLK -|2      17|- GRD2
        STB -|3      16|- GND
         K2 -|4      15|- GRD3
        VDD -|5      14|- GRD4
   SEG1/KS1 -|6      13|- SEG14/GRD5
   SEG2/KS2 -|7      12|- SEG13/GRD6
   SEG3/KS3 -|8      11|- SEG12/GRD7
   SEG4/KS4 -|9      10|- SEG5/KS5
             +---------+
*/

#include "TM1618Anode.h"

TM1618Anode::TM1618Anode(byte dataPin, byte clockPin, byte strobePin, byte numDigits, bool activateDisplay, byte intensity)
	: TM1618(dataPin, clockPin, strobePin, numDigits, activateDisplay, intensity)
{
  _maxSegments=TM1618Anode_MAX_SEG;
  //_maxSegments=__min(12-numDigits, TM1618Anode_MAX_SEG);  // better???
  _maxDisplays=TM1618Anode_MAX_POS;
  //_maxDisplays=__min(numDigits, TM1618Anode_MAX_POS);  // better???

  // NOTE: CONSTRUCTORS SHOULD NOT CALL DELAY(), MILLIS() OR MICROS() <= may hang ESP8266/ESP32/LGT8F328P/CH32V003/STM32...
}

void TM1618Anode::setupDisplay(bool active, byte intensity)
{
  // Set display mode. For TM1618Anode, the display mode is reverse of regular common cathode usage
  // Upper grid pins are shared with the upper segment pins, so select appropriate mode based on _maxSegments
  sendCommand(_maxSegments==7 ? TM16XX_CMD_MODE_7GRID : (_maxSegments==6 ? TM16XX_CMD_MODE_6GRID : (_maxSegments==5 ? TM16XX_CMD_MODE_5GRID : TM16XX_CMD_MODE_4GRID)));

  // Switch display on/off and set intensity 
  sendCommand(TM16XX_CMD_DISPLAY | (active ? 8 : 0) | min(7, intensity));
}

void TM1618Anode::setSegmentMap(const byte aMap[])
{ // Set a segment map to be used in subsequent setting of segments
  // Example map to move all segments except G and DP
  // byte aSegmentMap[]={3, 2, 1, 0, 5, 4, 6, 7};
  pSegmentMap=aMap;
}

byte TM1618Anode::mapSegments(byte segments)
{ // Map the segments to another location if that's requested.
  // The segment map is an array of _maxSegments long of which each element specfies the remapped position.
  // Usually segment A is mapped to pin SEG1/GRID1 (for cathode/anode usage). Using segmentmapping this can become any other pin.
  if(pSegmentMap)
  {
    byte nSegmentsMapped=0;
    for(byte n=0; n<_maxSegments; n++)
      nSegmentsMapped|=((segments&bit(n))?bit(pSegmentMap[n]):0);
    segments=nSegmentsMapped;
  }
  return(segments);
}

void TM1618Anode::setSegments(byte segments, byte position)
{	// Set leds on common anode GRID/SEG lines as specified.
  // TM1618 in common anode mode supports up to 7 GRID lines for segment anodes, connected to max. 5 SEG lines for cathodes
  // since the segments of digit are located at different display addresses, a memory bitmap is used to transpose the segments.

  // Map segments if specified for alternative segment wiring.
  segments=mapSegments(segments);
    
	//if(position<_maxDisplays)
	if(position<TM1618Anode_MAX_POS)
	{
		//update our memory bitmap
		this->bitmap[position]=segments;

		// Transpose the segments/positions to counter Common Anode connections and send the whole bitmap 
 		for (byte nSeg = 0; nSeg < _maxSegments; nSeg++)
 		{
 		  byte nVal=0;
   		for(byte nPos=0; nPos < TM1618Anode_MAX_POS; nPos++)
			{
				// Assume 1st digit to be connected to SEG1, 2nd digit connected to SEG2 etc
				nVal |= (((this->bitmap[nPos] >> nSeg) & 1) << (nPos));
			}
			
      // Since the TM1618 uses two bytes per position, the (transposed) segments are sent in two parts,
      // using nSeg as position and the combined byte as segment value.
      // We cannot call the parent method TM1618::setSegments() because it has an unwanted check.
      sendData(nSeg << 1, (byte)nVal&0x1F);
      sendData((nSeg << 1) | 1, (byte)(nVal>>5)<<3);
 		}
 	}
}

void TM1618Anode::clearDisplay()
{ // NOTE: TM16xx class assumes chips only have 2 bytes per digit when it uses >8 segments
	memset(this->bitmap, 0, TM1618Anode_MAX_POS); // clear the memory bitmap
  TM1618::clearDisplay();  // call parent method
}