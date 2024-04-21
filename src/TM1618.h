/*
TM1618 - Library for TM1618 led display driver.
TM1618: 5x7/8x4 SEGxGRD, 5 x 1 single button, DIO/CLK/STB

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment displays and use Adafruit GFX on matrix displays.

Made by Maxint R&D. See https://github.com/maxint-rd
*/

#ifndef TM1618_h
#define TM1618_h

#include "TM16xx.h"

#define TM1618_MAX_POS 7

class TM1618 : public TM16xx
{
  public:
    /** Instantiate a TM1618 module specifying data, clock and stobe pins, the number of digits, the display state, the starting intensity (0-7). */
    TM1618(byte dataPin, byte clockPin, byte strobePin, byte numDigits=4, bool activateDisplay = true, byte intensity = 7);

   /** Set the display (segments and LEDs) active or off and intensity (range from 0-7). */
   virtual void setupDisplay(bool active, byte intensity);   // For TM1618: also set the display mode (based on _maxSegments)

		/** Set the segments at a specific position on or off */
	  virtual void setSegments(byte segments, byte position);

		/** Clear the display */
		virtual void clearDisplay();

    /** Returns the pressed buttons as a bit set (left to right). */
    virtual uint32_t getButtons();
};

#endif