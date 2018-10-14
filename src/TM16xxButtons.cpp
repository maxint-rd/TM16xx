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

*/

#include "TM16xxButtons.h"

// constructor
TM16xxButtons::TM16xxButtons(TM16xx *pTM16xx)
{
	// TODO: safe memory by using dynamic memory allocation instead of static arrays for button states

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
void TM16xxButtons::setPressTicks(int ticks)
{
  _pressTicks = ticks;
} // setPressTicks


// save function for click event
void TM16xxButtons::attachClick(callbackTM16xxButtons newFunction)
{
  _clickFunc = newFunction;
} // attachClick


// save function for doubleClick event
void TM16xxButtons::attachDoubleClick(callbackTM16xxButtons newFunction)
{
  _doubleClickFunc = newFunction;
} // attachDoubleClick


// save function for longPressStart event
void TM16xxButtons::attachLongPressStart(callbackTM16xxButtons newFunction)
{
  _longPressStartFunc = newFunction;
} // attachLongPressStart

// save function for longPressStop event
void TM16xxButtons::attachLongPressStop(callbackTM16xxButtons newFunction)
{
  _longPressStopFunc = newFunction;
} // attachLongPressStop

// save function for during longPress event
void TM16xxButtons::attachDuringLongPress(callbackTM16xxButtons newFunction)
{
  _duringLongPressFunc = newFunction;
} // attachDuringLongPress

// function to get the current long pressed state
bool TM16xxButtons::isLongPressed(byte nButton){
  return _isLongPressed[nButton];
}

int TM16xxButtons::getPressedTicks(byte nButton){
  return _stopTime[nButton] - _startTime[nButton];
}

void TM16xxButtons::reset(void)
{
  for(byte n=0; n<TM16XX_BUTTONS_MAXBUTTONS; n++)
  {
	  _state[n] = 0; // restart.
	  _startTime[n] = 0;
	  _stopTime[n] = 0;
	  _isLongPressed[n] = false;
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

  if (_state[nButton] == 0) { // waiting for menu pin being pressed.
    if (activeLevel) {
      _state[nButton] = 1; // step to state 1
      _startTime[nButton] = now; // remember starting time
    } // if

  } else if (_state[nButton] == 1) { // waiting for menu pin being released.
/*
    if ((!activeLevel) &&
        ((unsigned long)(now - _startTime) < _debounceTicks)) {
      // button was released to quickly so I assume some debouncing.
      // go back to state 0 without calling a function.
      _state = 0;

    } else
*/
    if (!activeLevel) {
      _state[nButton] = 2; // step to state 2
      _stopTime[nButton] = now; // remember stopping time

    } else if ((activeLevel) &&
               ((unsigned long)(now - _startTime[nButton]) > _pressTicks)) {
      _isLongPressed[nButton] = true; // Keep track of long press state
      if (_longPressStartFunc)
        _longPressStartFunc(nButton);
      if (_duringLongPressFunc)
        _duringLongPressFunc(nButton);
      _state[nButton] = 6; // step to state 6
      _stopTime[nButton] = now; // remember stopping time
    } else {
      // wait. Stay in this state.
    } // if

  } else if (_state[nButton] == 2) {
    // waiting for menu pin being pressed the second time or timeout.
    if (_doubleClickFunc == NULL ||
        (unsigned long)(now - _startTime[nButton]) > _clickTicks) {
      // this was only a single short click
      if (_clickFunc)
        _clickFunc(nButton);
      _state[nButton] = 0; // restart.

    } else if ((activeLevel))
    	 // && ((unsigned long)(now - _stopTime) > _debounceTicks))
    	 {
      _state[nButton] = 3; // step to state 3
      _startTime[nButton] = now; // remember starting time
    } // if

  } else if (_state[nButton] == 3) { // waiting for menu pin being released finally.
    // Stay here for at least _debounceTicks because else we might end up in
    // state 1 if the button bounces for too long.
    if ((!activeLevel))
    	// && ((unsigned long)(now - _startTime) > _debounceTicks))
    	{
      // this was a 2 click sequence.
      if (_doubleClickFunc)
        _doubleClickFunc(nButton);
      _state[nButton] = 0; // restart.
      _stopTime[nButton] = now; // remember stopping time
    } // if

  } else if (_state[nButton] == 6) {
    // waiting for menu pin being release after long press.
    if (!activeLevel) {
      _isLongPressed[nButton] = false; // Keep track of long press state
      if (_longPressStopFunc)
        _longPressStopFunc(nButton);
      _state[nButton] = 0; // restart.
      _stopTime[nButton] = now; // remember stopping time
    } else {
      // button is being long pressed
      _isLongPressed[nButton] = true; // Keep track of long press state
      if (_duringLongPressFunc)
        _duringLongPressFunc(nButton);
    } // if

  } // if
} // OneButton.tick(nButton)
