/*

TM16xxButtons.cpp - Buttons class for TM16xx.
The TM16xxButtons class supports the key-scanning features of TM16xx chips, such as TM1637 or TM1638. 
It extends the getButtons() function of the base class and provides these features:
 - setting callback functions
 - multi-state keys (similar to OneButton): Press, LongPress, Click, Doubleclick

These are some TM16xx chips that support key-scanning:
   TM1637   8 x 2 single    DIO/CLK
   TM1638   8 x 3 multi     DIO/CLK/STB
   TM1668   10 x 2 multi    DIO/CLK/STB

Made by Maxint R&D. See https://github.com/maxint-rd/
Partially based on OneButton library by Matthias Hertel. See https://github.com/mathertel/OneButton

*/

#include "TM16xxButtons.h"

// constructor
TM16xxButtons::TM16xxButtons(TM16xx *pTM16xx)
{
	// TODO: reduce memory by using dynamic memory allocation instead of static arrays for button states
	// requires additional constructor parameter to allow less than TM16XX_BUTTONS_MAXBUTTONS

	_pTM16xx=pTM16xx;
	reset();
}



// explicitly set the number of millisec that have to pass by before a click is
// detected.
void TM16xxButtons::setClickTicks(int ticks)
{
  _clickTicks = ticks;
} // setClickTicks


// explicitly set the number of millisec that have to pass by before a long
// button press is detected.
void TM16xxButtons::setLongPressTicks(int ticks)
{
  _longPressTicks = ticks;
} // setLongPressTicks

// set function for release event
void TM16xxButtons::attachRelease(callbackTM16xxButtons newFunction)
{
  _releaseFunc = newFunction;
} // attachRelease

// set function for click event
void TM16xxButtons::attachClick(callbackTM16xxButtons newFunction)
{
  _clickFunc = newFunction;
} // attachClick

// set function for doubleClick event
void TM16xxButtons::attachDoubleClick(callbackTM16xxButtons newFunction)
{
  _doubleClickFunc = newFunction;
} // attachDoubleClick


// set function for longPressStart event
void TM16xxButtons::attachLongPressStart(callbackTM16xxButtons newFunction)
{
  _longPressStartFunc = newFunction;
} // attachLongPressStart

// set function for longPressStop event
void TM16xxButtons::attachLongPressStop(callbackTM16xxButtons newFunction)
{
  _longPressStopFunc = newFunction;
} // attachLongPressStop

// set function for during longPress event
void TM16xxButtons::attachDuringLongPress(callbackTM16xxButtons newFunction)
{
  _duringLongPressFunc = newFunction;
} // attachDuringLongPress

// function to get the current long pressed state
bool TM16xxButtons::isPressed(byte nButton)
{
  return(_state[nButton]==TM16XX_BUTTONS_STATE_PRESSED || _state[nButton]==TM16XX_BUTTONS_STATE_DBLPRESS || _state[nButton]==TM16XX_BUTTONS_STATE_LPRESS);
}

// function to get the current long pressed state
bool TM16xxButtons::isLongPressed(byte nButton)
{
	return(_state[nButton]==TM16XX_BUTTONS_STATE_LPRESS);
}

int TM16xxButtons::getPressedTicks(byte nButton)
{
  return _stopTime[nButton] - _startTime[nButton];
}

void TM16xxButtons::reset(void)
{
  for(byte n=0; n<TM16XX_BUTTONS_MAXBUTTONS; n++)
  {
	  _state[n] = TM16XX_BUTTONS_STATE_START; // restart.
	  _startTime[n] = 0;
	  _stopTime[n] = 0;
	}
}


/**
 * @brief Check input of the configured pin and then advance the finite state
 * machine (FSM).
 */
uint32_t TM16xxButtons::tick(void)
{	// update the state of each button and call callback functions as needed
	uint32_t dwButtons=_pTM16xx->getButtons();
#ifdef TM16XX_DEBUG
  Serial.print(F("TM16xxButtons: "));
  Serial.print(dwButtons, HEX);
  Serial.print(F(", state: "));
  for(byte n=0; n<TM16XX_BUTTONS_MAXBUTTONS; n++)
  {
	  tick(n, dwButtons&_BV(n));
	  Serial.print(_state[n]);
	}
  Serial.print(F("    "));
#else
  for(byte n=0; n<TM16XX_BUTTONS_MAXBUTTONS; n++)
	  tick(n, dwButtons&_BV(n));
#endif
	return(dwButtons);
}

/**
 * @brief Advance the finite state machine (FSM) using the given level.
 */
void TM16xxButtons::tick(byte nButton, bool activeLevel)
{
  unsigned long now = millis(); // current (relative) time in msecs.

  // Implementation of the state machine
  switch(_state[nButton])
  {
  case TM16XX_BUTTONS_STATE_START:	// waiting for button being pressed.
    if (activeLevel)
    {
      _state[nButton] = TM16XX_BUTTONS_STATE_PRESSED; // step to pressed state
      _startTime[nButton] = now; // remember starting time
    } // if
    break;

  case TM16XX_BUTTONS_STATE_PRESSED: // waiting for button being released.
    if (!activeLevel)
    {
      _state[nButton] = TM16XX_BUTTONS_STATE_RELEASED; // step to released state
      _stopTime[nButton] = now; // remember stopping time
      if (_releaseFunc)
        _releaseFunc(nButton);
    }
    else if ((activeLevel) && ((unsigned long)(now - _startTime[nButton]) > _longPressTicks))
    {
      _state[nButton] = TM16XX_BUTTONS_STATE_LPRESS; // step to long press state
      _stopTime[nButton] = now; // remember stopping time
      if (_longPressStartFunc)
        _longPressStartFunc(nButton);
      if (_duringLongPressFunc)
        _duringLongPressFunc(nButton);
    } else {
      // wait. Stay in this state.
    } // if
    break;

  case TM16XX_BUTTONS_STATE_RELEASED: // waiting for button being pressed the second time or timeout.
    if (_doubleClickFunc == NULL || (unsigned long)(now - _startTime[nButton]) > _clickTicks)
    {
      // this was only a single short click
      if (_clickFunc)
        _clickFunc(nButton);
      _state[nButton] = TM16XX_BUTTONS_STATE_START; // restart.
    }
    else if ((activeLevel))
    {
      _state[nButton] = TM16XX_BUTTONS_STATE_DBLPRESS; // step to doubleclick state
      _startTime[nButton] = now; // remember starting time
    } // if
    break;

  case TM16XX_BUTTONS_STATE_DBLPRESS: // waiting for button being released finally.
    if ((!activeLevel))
   	{
      // this was a 2 click sequence.
      _state[nButton] = TM16XX_BUTTONS_STATE_START; // restart.
      _stopTime[nButton] = now; // remember stopping time
      if (_releaseFunc)
        _releaseFunc(nButton);
      if (_doubleClickFunc)
        _doubleClickFunc(nButton);
    } // if
    break;

  case TM16XX_BUTTONS_STATE_LPRESS: // waiting for button being release after long press.
    if (!activeLevel)
    {
      _state[nButton] = TM16XX_BUTTONS_STATE_START; // restart.
      _stopTime[nButton] = now; // remember stopping time
      if (_releaseFunc)
        _releaseFunc(nButton);
      if (_longPressStopFunc)
        _longPressStopFunc(nButton);
    }
    else
    {
      // button is being long pressed
      if (_duringLongPressFunc)
        _duringLongPressFunc(nButton);
    } // if
		break;
  } // switch
} // TM16xxButtons.tick(nButton)
