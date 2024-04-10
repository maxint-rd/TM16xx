/*
TM1668.h - Library for TM1668.

Copyright (C) 2011 Ricardo Batista <rjbatista at gmail dot com>
Adjusted for TM1668 by Maxint R&D, based on TM1638 code. See https://github.com/maxint-rd/

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


TM1668 support by Maxint-RD @MMOLE, based on TM1638 class

For the TM1668 module the following default setup is assumed:
 - display mode 7x10 (ie. 10seg, 7grd)
 - 4 digits 7-segment+dp (common cathode) connected to GRD1-GRD4 and SEG1-SEG8 (max 7 digits, connected to GRD1-GRD7)
 - max 10x RGB LEDs (common anode) connected to GRD5-GRD7 and SEG1-SEG10
 - max 10x2 buttons connected via diodes to K1-K2 and  SEG1-SEG10 (=KS1-KS10)
 - max 4x2 RG LEDs (common cathode) connected to GRD1-GRD4 and SEG9-SEG10

*/

#ifndef TM1668_h
#define TM1668_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "TM16xx.h"

#define TM1668_COLOR_NONE   0
#define TM1668_COLOR_RED    1
#define TM1668_COLOR_GREEN  2
#define TM1668_COLOR_BLUE   4
#define TM1668_COLOR_YELLOW    (TM1668_COLOR_RED|TM1668_COLOR_GREEN)
#define TM1668_COLOR_PURPLE    (TM1668_COLOR_RED|TM1668_COLOR_BLUE)
#define TM1668_COLOR_AQUA    (TM1668_COLOR_GREEN|TM1668_COLOR_BLUE)
#define TM1668_COLOR_WHITE    (TM1668_COLOR_RED|TM1668_COLOR_GREEN|TM1668_COLOR_BLUE)

#define TM1668_POS_RED 4
#define TM1668_POS_GREEN 5
#define TM1668_POS_BLUE 6

#define TM1668_DISPMODE_4x13 0
#define TM1668_DISPMODE_5x12 1
#define TM1668_DISPMODE_6x11 2
#define TM1668_DISPMODE_7x10 3

/*
#define TM1668_KEYSET_ALL 0
#define TM1668_KEYSET_K1 1
#define TM1668_KEYSET_K2 2
*/

class TM1668 : public TM16xx
{
  public:
    /** Instantiate a TM1668 module specifying the display state, the starting intensity (0-7) data, clock and stobe pins. */
    TM1668(byte dataPin, byte clockPin, byte strobePin, byte numDigits = 4, bool activateDisplay = true, byte intensity = 7, byte displaymode = TM1668_DISPMODE_7x10);

		/** Set the segments at a specific position on or off */
	  virtual void setSegments(byte segments, byte position);
	  virtual void setSegments16(uint16_t segments, byte position);

    /** Set the LED at pos to color (TM1668_COLOR_RED, TM1668_COLOR_GREEN or both) */
    virtual void setLED(byte color, byte pos);
    /** Set the LEDs. MSB byte for the green LEDs, LSB for the red LEDs */
		void setLEDs(uint32_t leds);
		void setRGBLEDs(uint32_t uRgbLeds);
		void setRGBLED(byte color, byte pos);
	
    /** Returns the pressed buttons as a bit set (left to right), K2|K1 or just K1 or K2. */
    //virtual uint32_t getButtons(byte keyset);
    //virtual uint32_t getButtons();		// calls getButtons(TM1668_KEYSET_ALL)
    virtual uint32_t getButtons();		// returns K1 keys in low word, K2 keys in high word

  protected:
		uint32_t _uTenRgbLeds;
};

#endif
