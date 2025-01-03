/*

TM16xxDisplay.h - LED Display class for TM16xx.
The TM16xxDisplay class supports 7-segment LED displays of various sizes to be
connected to a TM16xx chip, such as TM1637 or TM1640. 

These are the dimensions supported by the tested TM16xx chips:
   TM1637  8x6   (common anode)
   TM1638  10x8  (common cathode)
   TM1640  8x16  (common cathode)
   TM1650  8x4   (common cathode)   (mode 7x4 not tested yet)
   TM1668  10x7  (common cathode)   (not supported yet: modes 11x6, 12x5, 13x4)

Made by Maxint R&D. See https://github.com/maxint-rd/

*/

#ifndef _TM16XX_DISPLAY_H
#define _TM16XX_DISPLAY_H

#include "TM16xx.h"
#include <Print.h>
//#include "TM16xxFonts.h"

// Set TM16XX_OPT_COMBIDISPLAY to 1 to support using print() on combined displays. Set it to 0 to save some memory.
#define TM16XX_OPT_COMBIDISPLAY 1

class TM16xxDisplay : public Print
{
 public:
  TM16xxDisplay(TM16xx *pTM16xx, byte nNumDigits);
#if(TM16XX_OPT_COMBIDISPLAY)
  TM16xxDisplay(TM16xx *apTM16xx[], byte nNumModules, byte nNumDigitsTotal);
#endif
  void setIntensity(byte intensity);		// intensity 0-7, 0=off, 7=bright

  virtual void clear();

  /** sets flipped state of the display (every digit is rotated 180 degrees) */
  virtual void setDisplayFlipped(bool flipped);

  // Set the display to the String (defaults to built in font)
  virtual void setDisplayToString(const char* string, const word dots=0, const byte pos=0, const byte font[] = TM16XX_FONT_DEFAULT);
  virtual void setDisplayToString(String string, const word dots=0, const byte pos=0, const byte font[] = TM16XX_FONT_DEFAULT);
  virtual void setDisplayToError();

  // Set the display to a unsigned hexadecimal number (with or without leading zeros)
  void setDisplayToHexNumber(unsigned long number, byte dots, bool leadingZeros = true, const byte numberFont[] = TM16XX_NUMBER_FONT);
  // Set the display to a unsigned decimal number (with or without leading zeros)
  void setDisplayToDecNumber(unsigned long number, byte dots, bool leadingZeros = true, const byte numberFont[] = TM16XX_NUMBER_FONT);
  // Set the display to a signed decimal number (with or without leading zeros)
  void setDisplayToSignedDecNumber(signed long number, byte dots, bool leadingZeros = true, const byte numberFont[] = TM16XX_NUMBER_FONT);
  // Set the display to a unsigned binary number
  void setDisplayToBinNumber(byte number, byte dots, const byte numberFont[] = TM16XX_NUMBER_FONT);

  // support for the Print class
  void setCursor(int8_t nPos); 	// allows setting negative to support scrolled printing
  using Print::write; // pull in write(str) and write(buf, size) from Print
  virtual size_t write(uint8_t character);
  /* virtual size_t write(const char *str); */

 protected:
  TM16xx *_pTM16xx;
  byte _nNumDigits;
  int8_t _nPrintPos=0;
  bool _fFlipped=false;

#if(TM16XX_OPT_COMBIDISPLAY)
  TM16xx *_apTM16xx[1];           // place to hold value when only one module is used
  TM16xx **_aModules;   // pointer to external array of modules
  byte _nNumModules=1;
#endif

 
 private:
  void setDisplayToDecNumberAt(unsigned long number, byte dots, byte startingPos, bool leadingZeros, const byte numberFont[]);
  //TM16xx *TM16xxDisplay::findModuleByPos(const byte nPosFind);
  void sendCharAt(const byte nPos, byte btData, bool fDot);
  void sendAsciiCharAt(const byte nPosCombi, char c, bool fDot, const byte font[] = TM16XX_FONT_DEFAULT);
};
#endif
