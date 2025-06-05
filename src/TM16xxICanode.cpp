/*
TM16xxICanode - Library implementation for TM16xxIC used in Common Anode configuration.

Tested configurations:
  TM1640: 16 GRD-segments x 8 SEG-digits 

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment displays and use Adafruit GFX on matrix displays.

Made by Maxint R&D. See https://github.com/maxint-rd
*/

#include "TM16xxICanode.h"

TM16xxICanode::TM16xxICanode(if_ctrl_tm16xx ctrl, byte dataPin, byte clockPin, byte strobePin, byte numDigits)
  : TM16xxIC(ctrl, dataPin, clockPin, strobePin, numDigits)
{
  // transpose SEG/GRD
  _maxDisplaysX=_maxSegments;
  _maxSegmentsX=16;
  if(_maxSegments>numDigits)    // correct _maxSegments for use of SEG pins as common anode
    _maxSegments=numDigits;
  _maxDisplays=ctrl.grid_len;   // max number of GRD pins usable for segments depends on display mode

  // NOTE: CONSTRUCTORS SHOULD NOT CALL DELAY(), MILLIS() OR MICROS() <= may hang ESP8266/ESP32/LGT8F328P/CH32V003/STM32...
}

void TM16xxICanode::setupDisplay(bool active, byte intensity)
{
  // TODO: Set display mode. For TM16xxICanode, the display mode is reverse of regular common cathode usage
  // Upper grid pins are shared with the upper segment pins, so select appropriate mode based on _maxSegments
  //sendCommand(_maxSegments==7 ? TM16XX_CMD_MODE_7GRID : (_maxSegments==6 ? TM16XX_CMD_MODE_6GRID : (_maxSegments==5 ? TM16XX_CMD_MODE_5GRID : TM16XX_CMD_MODE_4GRID)));

  // Switch display on/off and set intensity 
  //sendCommand(TM16XX_CMD_DISPLAY | (active ? 8 : 0) | min(7, intensity));
  TM16xxIC::setupDisplay(active, intensity);
}

void TM16xxICanode::setAlphaNumeric(bool fAlpha, const byte *pMap)    // const byte aMap[]
{
  pSegmentMapX=pMap;  // store pointer for mapping in derived version of setSegments16
  TM16xxIC::setAlphaNumeric(fAlpha, NULL); // prevent parent from mapping segments since we do it in mapSegments
}


/*
byte TM16xxICanode::mapSegments(byte segments)
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
*/

void TM16xxICanode::setSegments16(uint16_t segments, byte position)
{ // Set leds on common anode GRID/SEG lines as specified.
  // TM1640 in common anode mode supports 16 GRID lines for segment anodes, connected to max. 8 SEG lines for cathodes
  // Since the segments of digits are located at different display addresses, a memory bitmap is used to transpose the segments.

  // Since sendData() does implicit begin(), it may clear the bitmap after we filled it
  // To avoid this we call begin() first to ensure it's not executed later.
  begin();     // calls setupDisplay() and clearDisplay(), which will clear the bitmap 

  //if(position<_maxDisplays)
  if(position<TM16xxICanode_MAX_POS)
  {
    // Map segments if specified for alternative segment wiring.
    if(pSegmentMapX)
      segments=mapSegments16(segments, pSegmentMapX);

    // update our memory bitmap, remember changed segments
    uint16_t uChangedSegments=bitmap[position] ^ segments;      // determine changed segments (using xor) to minimize data traffic 
    this->bitmap[position]=segments;

    // Transpose the segments/positions to counter Common Anode connections and send the whole bitmap 
    for (byte nSeg = 0; nSeg < _maxSegmentsX; nSeg++)      // transpose _maxSegments/_maxDisplays
    {
      if(uChangedSegments & bit(nSeg))
      { // Update the display, but only for changed segments.
        // Updating all segments is very slow since for every digit the whole display will be updated
        // Testing scrolled text on TM1640 16 segments x 8 CA-digits @8MHz CH32 showed 727ms per 8-char line when sending all segments. 
        // Sending only changed was 0-320ms, depending on contents. @48MHz measurements were 0-67ms.
        uint16_t nVal=0;
        for(byte nPos=0; nPos < _maxDisplaysX; nPos++)
        {
          // Assume 1st digit to be connected to SEG1, 2nd digit connected to SEG2 etc
          nVal |= (((this->bitmap[nPos] >> nSeg) & 1) << (nPos));
        }

        // Send the transposed position data for the segment.
        TM16xxIC::setSegments16(nVal, nSeg);
      }
    }
  }
}

void TM16xxICanode::clearDisplay()
{ // NOTE: TM16xx class assumes chips only have 2 bytes per digit when it uses >8 segments
  TM16xxIC::clearDisplay();  // call parent method 
  memset(this->bitmap, 0, TM16xxICanode_MAX_POS*sizeof(this->bitmap[0])); // clear the memory bitmap
}