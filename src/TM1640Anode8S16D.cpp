/*
TM1640Anode8S16D - Library implementation for TM1640 with 2 x 8-SEG x 8-digit Common Anode configuration.
Such 16 digit 7-segment CA configuration is mentioned on page 9 of the TM140B datasheet v1.2 (Chinese).
(See also issues #53 and #63 on https://github.com/maxint-rd/TM16xx).

On the 16 digit 7-segment Common Anode LED display modules there are two sets of digits.
The first 8 digits have their 7 segments plus dot connected to the GRD1-GRD8 lines of the TM1640.
The second 8 digits use the GRD9-GRD16 lines to drive the segments.
Both sets use SEG1-SEG8 as common anode line for the digits 1-8 within each set.
Using such layout there can be max. 16 CA digits.

Made by Maxint R&D, partly based on TM1640Anode class. See https://github.com/maxint-rd/
*/

#include "TM1640Anode8S16D.h"

TM1640Anode8S16D::TM1640Anode8S16D(byte dataPin, byte clockPin, byte numDigits, bool activateDisplay, byte intensity)
  : TM1640(dataPin, clockPin, numDigits, activateDisplay, intensity)
{
  _maxSegments=TM1640Anode8S16D_MAX_SEG;  
  _maxDisplays=TM1640Anode8S16D_MAX_POS;

  // NOTE: CONSTRUCTORS SHOULD NOT CALL DELAY() <= gives hanging on certain ESP8266 cores as well as on LGT8F328P
  // Using micros() or millis() in constructor also gave issues on LST8F328P and CH32.
  // Root cause is that the timer interrupt to update these counters may not be set until setup() is called.
  // To initialize the display an implicit begin() is called by TM16xx::sendData().
}


/*
void TM1640Anode8S16D::setSegmentMap(const byte aMap[])
{ // Set a segment map to be used in subsequent setting of segments
  // The map should be an 8-byte array, listing the target segment numbers (TODO: how about using a compressed 8-word array, ie. 32-bit value)
  pSegmentMap=aMap;
}

byte TM1640Anode8S16D::mapSegments(byte segments)
{ // Map the segments to another location if that's requested.
  // The segment map is an array of _maxSegments long of which each element specfies the remapped position.
  // Usually segment A is mapped to pin GRID1 of the TM1640. Using segmentmapping this can become any other pin.
  if(pSegmentMap)
  {
    byte nSegmentsMapped=0;
    for(byte n=0; n<_maxSegments; n++)
    {
      nSegmentsMapped|=((segments&bit(n))?bit(pSegmentMap[n]):0);
    }
    segments=nSegmentsMapped;
  }
  return(segments);
}
*/

void TM1640Anode8S16D::setSegments(byte segments, byte position)
{ // Set 2x8 leds on common grd as specified.
  // Update memory, transpose and send it to display.
  // TM1640 uses 8 SEGs for 16 GRIDs one byte address per GRID.
  // For the two sets of 7-segment displays in common anode configuration one SEG per display position is used.

  if(position<TM1640Anode8S16D_MAX_POS)
  {
    begin();    // implicit begin() now, to avoid clearing a partially filled memory bitmap in sendData().

    // Map segments if specified for alternative segment wiring.
    //segments=mapSegments(segments);

    // update our memory bitmap, remember changed segments
    byte uChangedSegments=bitmap[position] ^ segments;      // determine changed segments (using xor) to minimize data traffic 
    this->bitmap[position]=segments;

    // Transpose the segments/positions to counter Common Anode connections and send the updated memory bitmap.
    // Only the block of the memory bitmap that has the specifed position needs to be sent.
    for (byte nSeg = 0; nSeg < _maxSegments; nSeg++)
    {
      if(uChangedSegments & bit(nSeg))
      { // Update the display, but only for changed segments.
        byte nVal=0;
        byte nBlockStart=(position>7?8:0);
        byte nBlockEnd=(position>7?TM1640Anode8S16D_MAX_POS:8);
        for(byte nPos=nBlockStart; nPos < nBlockEnd; nPos++)
        {
          // Assume 1st digit to be connected to SEG1, 2nd digit connected to SEG2 etc
          nVal |= (((this->bitmap[nPos] >> nSeg) & 1) << (nPos%8));
        }
        sendData(nSeg+nBlockStart, nVal);
      }
    }
  }
}

void TM1640Anode8S16D::clearDisplay()
{ // clear the memory bitmap and the display
  memset(this->bitmap, 0, TM1640Anode8S16D_MAX_POS*sizeof(uint8_t));
  for(byte nPos=0; nPos<TM1640Anode8S16D_MAX_POS; nPos++)
  { // set all segments OFF
    sendData(nPos, 0);
  }
}

