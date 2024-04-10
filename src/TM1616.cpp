/*
TM1616.h - Library for TM1616. DIN/CLK/STB, 7x4 LED, no buttons.
Made by Maxint R&D. See https://github.com/maxint-rd/TM16xx
*/

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM1616.h"

TM1616::TM1616(byte dataPin, byte clockPin, byte strobePin, byte numDigits, bool activateDisplay, byte intensity)
	: TM16xx(dataPin, clockPin, strobePin, TM1616_MAX_POS, numDigits, activateDisplay, intensity)
{
  _maxSegments=7;

  // Display mode command setting
  // Upper two grid pins are shared with the upper two segment pins, so select appropriate mode based on numDigits
  // TM1616 only supports 7x4 mode, which still needs to be specified to use the display
  sendCommand(0x00); // display mode 7x4

	clearDisplay();
	setupDisplay(activateDisplay, intensity);
}

void TM1616::setSegments(byte segments, byte position)
{	// set segment leds on common grd as specified
  // TM1616 uses only 7 segments, but still has two bytes of memory, like TM1616
  // for the digit displays only the first byte (containing seg1-seg8) is actually used.
	// Only the LSB (SEG1-8) is sent to the display
	if(position<_maxDisplays)
		sendData(position << 1, segments);
}

void TM1616::clearDisplay()
{ // need to override clearDisplay to accommodate the memory layout used by TM1616
  for (byte nPos = 0; nPos < _maxDisplays; nPos++)
    setSegments(0x00, nPos);
}
