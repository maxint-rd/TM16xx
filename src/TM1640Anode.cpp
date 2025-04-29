/*
TM1640Anode.cpp - Library implementation for TM1640 with Common Anode up to 8 digits of 16 segments.

5241BS - 2 digits of 14 segments plus dot:

   |---A---|
   |\  |  /|
   F H J K B
   |  \|/  |
   |-G1-G2-|
   |  /|\  |
   E N M L C 
   |/  |  \|
   |---D---| DP
 1
   
Pinout (bottom side up)
  | | | | | | | | |            Index Segment Pin             Index Segment Pin
  | | | | | | | | |            [0]         A  12             [ 7]       DP   8 
  1 2 3 4 5 6 7 8 9            [1]         B  10             [ 9]        H  17
+-|-|-|-|-|-|-|-|-|-+          [2]         C   9             [10]        J  15
| o o o o o o o o o |          [3]         D   7             [11]        K  14
|                   |          [4]         E   1             [12]        L   5
|  5241BS (Com.An.) |          [5]         F  18             [13]        M   4
|                   |          [6]        G1  13             [14]        N   2
| o o o o o o o o o |          [8]        G2   6
+-|-|-|-|-|-|-|-|-|-+
  1 1 1 1 1 1 1 1 1         Pin 3 is not connected
  8 7 6 5 4 3 2 1 0         Anode digit 1 is on pin 16
  | | | | | | | | |         Anode digit 2 is on pin 11

Made by Maxint R&D, partly based on TM1638Anode class. See https://github.com/maxint-rd/
*/

#include "TM1640Anode.h"

TM1640Anode::TM1640Anode(byte dataPin, byte clockPin, byte numDigits, bool activateDisplay, byte intensity)
  : TM1640(dataPin, clockPin, numDigits, activateDisplay, intensity)
{
  _maxSegments=TM1640Anode_MAX_SEG;   // On the 5241BS LED display modules the 15 segments are connected to the GRD1-GRD16 lines of the TM1640.
                                      // The display has the two common anode digits connected to the SEG lines, resulting in max. 8 digits.
  _maxDisplays=TM1640Anode_MAX_POS;
/*
  clearDisplay();
  setupDisplay(activateDisplay, intensity);
*/
  // NOTE: CONSTRUCTORS SHOULD NOT CALL DELAY() <= gives hanging on certain ESP8266 cores as well as on LGT8F328P
  // Using micros() or millis() in constructor also gave issues on LST8F328P and CH32.
  // Root cause is that the timer interrupt to update these counters may not be set until setup() is called.
  // To initialize the display an implicit begin() is called by TM16xx::sendData().
}

void TM1640Anode::sendAsciiChar(byte pos, char c, bool fDot, const byte font[])
{ // Method to send an Ascii character to the display.
  // This method is also called by TM16xxDisplay.print() to display characters.
  // The base class uses the default 7-segment font to find the LED pattern.
  // Derived classes for multi-segment displays or alternate layout displays can override this method.
  uint16_t uSegments= pgm_read_word(TM16XX_FONT_15SEG+(c - 32));
  setSegments16(uSegments | (fDot ? 0b10000000 : 0), pos);

  // Note: It is assumed that TM1640Anode::sendAsciiChar() is always used in 16-SEG x 8-GRD (in Common Anode configuration)
}

void TM1640Anode::setSegments(byte segments, byte position)
{ // Set regular 7-segment segments on the 15-segment display. This allows for using the regular 7-segment font.
  // On the 5241BS 15-segment display, segment G is split into G1 and G2. Assume these to be segments 6 and 8, having DP on segment 7 (index 0)
  uint16_t segments16=segments;
  if(segments16&bit(6))
    segments16|=bit(8);   // duplicate G1 to G2
  this->setSegments16((uint16_t)segments16, position);
}

void TM1640Anode::setSegmentMap(const byte aMap[])
{ // Set a segment map to be used in subsequent setting of segments
  // Example map for easy breadboard wiring of TM1650 to 5241BS (TM1640 pin 1 right-top, 5241BS pin 1 left bottom)
  // byte aSegmentMap[]={12,11, 8, 6, 1, 9,13, 7, 5,10,15,14, 4, 3, 2, 0};       // mapping for TM1640 left of 5241BS with streight wiring
  pSegmentMap=aMap;
}

uint16_t TM1640Anode::mapSegments16(uint16_t segments)
{ // Map the segments to another location if that's requested.
  // The segment map is an array of _maxSegments long of which each element specfies the remapped position.
  // Usually segment A is mapped to pin GRID1 of the TM1640. Using segmentmapping this can become any other pin.
  if(pSegmentMap)
  {
    uint16_t nSegmentsMapped=0;
    for(byte n=0; n<_maxSegments; n++)
    {
      nSegmentsMapped|=((segments&bit(n))?bit(pSegmentMap[n]):0);
    }
    segments=nSegmentsMapped;
  }
  return(segments);
}

void TM1640Anode::setSegments16(uint16_t segments, byte position)
{ // Set 16 leds on common grd as specified
  // TM1640 uses 8 SEGs for 16 GRIDs one byte address per GRID
  // For the 15-segment displays in common anode configuration one SEG per display position is used.
  // When used as a 8-segment display, the G segment is translated to G1/G2.
  // Note: For 15-segment characters the sendAsciiChar function is redefined.

  //if(position<_maxDisplays)
  if(position<TM1640Anode_MAX_POS)
  {
    // Map segments if specified for alternative segment wiring.
    segments=mapSegments16(segments);

    // update our memory bitmap, remember changed segments
    uint16_t uChangedSegments=bitmap[position] ^ segments;      // determine changed segments (using xor) to minimize data traffic 
    this->bitmap[position]=segments;

    // Transpose the segments/positions to counter Common Anode connections and send the whole bitmap 
    for (byte nSeg = 0; nSeg < _maxSegments; nSeg++)
    {
      if(uChangedSegments & bit(nSeg))
      { // Update the display, but only for changed segments.
        byte nVal=0;
        for(byte nPos=0; nPos < TM1640Anode_MAX_POS; nPos++)
        {
          // Assume 1st digit to be connected to SEG1, 2nd digit connected to SEG2 etc
          nVal |= (((this->bitmap[nPos] >> nSeg) & 1) << (nPos));
        }
        sendData(nSeg, nVal);
      }
    }
  }
}

void TM1640Anode::clearDisplay()
{
  memset(this->bitmap, 0, TM1640Anode_MAX_POS*sizeof(uint16_t));
  for(byte nPos=0; nPos<TM1640Anode_MAX_POS; nPos++)
  { // set all segments OFF
    sendData(nPos << 1, 0);
    sendData((nPos << 1) | 1, 0);
  }
}
