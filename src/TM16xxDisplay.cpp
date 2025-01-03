/*

TM16xxDisplay.h - LED Display library for TM16xx.
Methods for driving 7-segment LED displays using TM16xx chips.

Made by Maxint R&D. See https://github.com/maxint-rd/
Based on TM1638/1640 library by Ricardo Batista.

*/
#include "TM16xxDisplay.h"
#include "string.h"

TM16xxDisplay::TM16xxDisplay(TM16xx *pTM16xx, byte nNumDigits)
{ // constructor for a display having only one module
	_pTM16xx=pTM16xx;
	_nNumDigits=nNumDigits;
#if(TM16XX_OPT_COMBIDISPLAY)
  // set number of modules to 1
 	_apTM16xx[0]=pTM16xx;   // use array allocated in class
	_aModules=_apTM16xx;
	_nNumModules=1;
#endif
}

#if(TM16XX_OPT_COMBIDISPLAY)
TM16xxDisplay::TM16xxDisplay(TM16xx *apTM16xx[], byte nNumModules, byte nNumDigitsTotal)
{ // constructor for a display combined out of one or more module
  _aModules=apTM16xx;   // use array provided
	_pTM16xx=apTM16xx[0];   // set default module to first module in array
	_nNumModules=nNumModules;
	_nNumDigits=nNumDigitsTotal;	// set n_numDigita to total in modules
}

/*
TM16xx *TM16xxDisplay::findModuleByPos(const byte nPosFind)
{ // find the module that holds the specified position
  //TM16xx *pModule=NULL;
  int nPosDone=0;
  for(int i=0; i<_nNumModules; i++)
  {
    if(nPosFind < nPosDone+_aModules[i]->_numDigits)
    {
      return(_aModules[i]);
    }
    nPosDone+=_aModules[i]->_numDigits;
  }
  return(NULL);
}
*/
#endif  // if(TM16XX_OPT_COMBIDISPLAY)

void TM16xxDisplay::sendCharAt(const byte nPosCombi, byte btData, bool fDot)
{ // set the specified data at specified position of the module that has that position)
  // If the display is flipped, the position should be flipped as a whole, but also position in the module (to compensate for flipping done in TM16xx).
  // Note that each individual module can have a different size (just to complicate things).
  byte nPos=nPosCombi;

  if(this->_fFlipped)
    nPos=_nNumDigits-1-nPos;      // flip the whole combination
#if(TM16XX_OPT_COMBIDISPLAY)
  for(int i=0; i<_nNumModules; i++)
  {
    if(nPos<0)
      return;
    if(nPos < _aModules[i]->getNumDigits())
    {
  	  _aModules[i]->sendChar(this->_fFlipped ? _aModules[i]->getNumDigits()-1-nPos : nPos, btData, fDot);      // compensate for flipping individual module
      return;
    }
    else
      nPos-=_aModules[i]->getNumDigits();
  }
#else
  _pTM16xx->sendChar(nPos, btData, fDot);
#endif
}

void TM16xxDisplay::sendAsciiCharAt(const byte nPosCombi, char c, bool fDot, const byte font[])
{ // Set the specified Ascii character at specified position of the module that has that position
  // If the display is flipped, the position should be flipped as a whole, but also position in the module (to compensate for flipping done in TM16xx).
  // Note that each individual module can have a different size (just to complicate things).
  byte nPos=nPosCombi;

  if(this->_fFlipped)
    nPos=_nNumDigits-1-nPos;      // flip the whole combination
#if(TM16XX_OPT_COMBIDISPLAY)
  for(int i=0; i<_nNumModules; i++)
  {
    if(nPos<0)
      return;
    if(nPos < _aModules[i]->getNumDigits())
    {
  	  _aModules[i]->sendAsciiChar(this->_fFlipped ? _aModules[i]->getNumDigits()-1-nPos : nPos, c, fDot, font);      // compensate for flipping individual module
      return;
    }
    else
      nPos-=_aModules[i]->getNumDigits();
  }
#else
  _pTM16xx->sendAsciiChar(nPos, c, fDot, font);
#endif  // #if(TM16XX_OPT_COMBIDISPLAY)
}

void TM16xxDisplay::setIntensity(byte intensity)
{	// set the intensity of the module; range 0-8, 0=off, 8=brightest
  if(intensity>8) intensity=8;
#if(TM16XX_OPT_COMBIDISPLAY)
  for(int i=0; i<_nNumModules; i++)
  { // set the insensity of each individual module
    // NOTE: when the inherited class has overloaded setupDisplay() with default extra parameters, 
    // this will call TM16xx::setupDisplay(), not TM16nn::setupDisplay(). (See TM1652.h)
    // Solution is to have derived classes use separate method for the call without the extra parameters
  	_aModules[i]->setupDisplay(intensity!=0, intensity>0 ? intensity-1: 0);
  }
#else
	_pTM16xx->setupDisplay(intensity!=0, intensity>0 ? intensity-1: 0);
#endif
}

void TM16xxDisplay::setDisplayFlipped(bool flipped)
{ // set flipped state of each module used in the display (every digit is rotated 180 degrees)
  // note: this only changes subsequent displayed characters, not the current 
  this->_fFlipped = flipped;
#if(TM16XX_OPT_COMBIDISPLAY)
  for(int i=0; i<_nNumModules; i++)
  {
  	_aModules[i]->setDisplayFlipped(flipped);
  }
#else
	_pTM16xx->setDisplayFlipped(flipped);
#endif
}


void TM16xxDisplay::setDisplayToString(const char* string, const word dots, const byte pos, const byte font[])
{	// call base implementation
	//_pTM16xx->setDisplayToString(string, dots, pos, font);
  int stringLength = strlen(string);

  for (int i = 0; i < _nNumDigits - pos; i++) {
    if (i < stringLength) {
  	  sendAsciiCharAt(i + pos, string[i], (dots & (1 << (_nNumDigits - i - 1))) != 0, font);   // use sendAsciiChar to support 14-segments (via derived classes)
    } else {
      break;
    }
  }
 }

void TM16xxDisplay::setDisplayToString(const String string, const word dots, const byte pos, const byte font[])
{	// additional implementation using String class (uses more memory than char * version)
  setDisplayToString(string.c_str(), dots, pos, font);
}

void TM16xxDisplay::setDisplayToError()
{	// set the display to Error text
  this->clear();
  _pTM16xx->setDisplay(TM16XX_ERROR_DATA, sizeof(TM16XX_ERROR_DATA));  // TODO?: support COMBIDISPLAY
}

void TM16xxDisplay::setDisplayToHexNumber(unsigned long number, byte dots, bool leadingZeros, const byte numberFont[])
{
  for (int nPos = 0; nPos < _nNumDigits; nPos++) {
    if (number > 0 || leadingZeros || nPos==0) {
      sendCharAt(_nNumDigits - nPos - 1, pgm_read_byte_near(numberFont + (number & 0xF)), (dots & (1 << nPos)) != 0);
    } else {
      sendCharAt(_nNumDigits - nPos - 1, 0, (dots & (1 << nPos)) != 0);     // clearDisplayDigit
    }
    number >>= 4;
	}
}

void TM16xxDisplay::setDisplayToDecNumberAt(unsigned long number, byte dots, byte startingPos, bool leadingZeros, const byte numberFont[])
{
  if (number > 99999999L) {
    setDisplayToError();    // original code: limit to 8 digit numbers
  } else {
    for (int nPos = 0; nPos < _nNumDigits - startingPos; nPos++) {
      if (number != 0 || nPos==0 || leadingZeros) {
        sendCharAt(_nNumDigits - nPos - 1, pgm_read_byte_near(numberFont + (number  % 10)), (dots & (1 << nPos)) != 0);
      } else {
        sendCharAt(_nNumDigits - nPos - 1, 0, (dots & (1 << nPos)) != 0);     // clearDisplayDigit
      }
      number /= 10;
    }
  }
}

void TM16xxDisplay::setDisplayToDecNumber(unsigned long number, byte dots, bool leadingZeros,	const byte numberFont[])
{
	setDisplayToDecNumberAt(number, dots, 0, leadingZeros, numberFont);
}

void TM16xxDisplay::setDisplayToSignedDecNumber(signed long number, byte dots, bool leadingZeros, const byte numberFont[])
{
	if (number >= 0) {
		setDisplayToDecNumberAt(number, dots, 0, leadingZeros, numberFont);
	} else {
		if (-number > 9999999L) {
			setDisplayToError();
		} else {
			setDisplayToDecNumberAt(-number, dots, 1, leadingZeros, numberFont);   // start at pos 1
      sendCharAt(0, MINUS, (dots & (0x80)) != 0); // minus at pos 0, use dot at highest bit = bit(7)
		}
	}
}

void TM16xxDisplay::setDisplayToBinNumber(byte number, byte dots, const byte numberFont[])
{
  for (int nPos = 0; nPos < _nNumDigits; nPos++) {
    sendCharAt(_nNumDigits - nPos - 1, pgm_read_byte_near(numberFont + ((number & (1 << nPos)) == 0 ? 0 : 1)), (dots & (1 << nPos)) != 0);
  }
}

void TM16xxDisplay::clear()
{
#if(TM16XX_OPT_COMBIDISPLAY)
  for(int i=0; i<_nNumModules; i++)
  {
  	_aModules[i]->clearDisplay();
  }
#else
	_pTM16xx->clearDisplay();
#endif
}
	
void TM16xxDisplay::setCursor(int8_t nPos)
{		// Set the print position. Allow negative numbers to support scrolling
	_nPrintPos=nPos;
}


/*
 * Support for the Print class
 *
 * See https://playground.arduino.cc/Code/Printclass
 *
*/
size_t TM16xxDisplay::write(uint8_t c)
{	//Code to display letter when given the ASCII code for it
  static uint8_t cPrevious=' ';		// remember last character prnted, to add a dot when needed
  static bool fPrevDot=false;     // remember last dot, for showing ...

  // first check for dot to possibly combine with previous character
  // note that dot at end might still fit, eg. "HELP."
  bool fDot=false;
  if((c=='.' || c==',' || c==':' || c==';') && _nPrintPos<=_nNumDigits && cPrevious!='\0')
  {
    if(fPrevDot)
      c=' ';
    else
    {
      if(_nPrintPos>0) _nPrintPos--; // use same position to display the dot
      c=cPrevious;
    }
    fDot=true;
    fPrevDot=true;
  }
  else
    fPrevDot=false;

  // handle end-of-line
  if(c=='\0' || c=='\n' || c=='\r' || _nPrintPos>=_nNumDigits)
  {
    while(_nPrintPos>0 && _nPrintPos<_nNumDigits)
    {	// clear the remainder of the line
      sendCharAt(_nPrintPos, 0, false);    // sending 0 is same as clearDisplayDigit()
      _nPrintPos++;
    }

    // MMOLE 211103: Returning zero from write should stop printing rest of the string.
    // However, on ESP32 print() won't stop when returning 0, so for compatibility we return 1.
    // and only reset the print position when we're at the end. This should work on all platforms.
    if(c=='\0' || c=='\n' || c=='\r')
      _nPrintPos=0;
    cPrevious='\0';
    fPrevDot=false;
    return(1);
  }

  // print character
  if(_nPrintPos>=0 && _nPrintPos<_nNumDigits)
    sendAsciiCharAt(_nPrintPos, c, fDot);
  cPrevious=c;
  _nPrintPos++;
  return(1);
}

/* MMOLE: not called by print()
size_t TM16xxDisplay::write(const char *str)
{
	Serial.println(F("write str"));
	setDisplayToString(str);
	_nPrintPos=0;
}
*/
