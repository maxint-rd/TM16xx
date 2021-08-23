/*

TM16xxButtons.cpp - Buttons class for TM16xx.
The TM16xxButtons class supports the key-scanning features of TM16xx chips, such as TM1637 or TM1638. 
It extends the getButtons() function of the base class and provides these features:
 - setting callback functions
 - multi-state keys (similar to OneButton): Press, LongPress, Click, Doubleclick
 - tracking button state of combined key presses

These are some TM16xx chips that support key-scanning:
   TM1628   10 x 2 multi    DIO/CLK/STB
   TM1630   7 x 1 multi     DIO/CLK/STB
   TM1637   8 x 2 single    DIO/CLK     (no support for combined key pressing according datasheet)
   TM1638   8 x 3 multi     DIO/CLK/STB
   TM1650   7 x 4 single    DIO/CLK
   TM1668   10 x 2 multi    DIO/CLK/STB

Made by Maxint R&D. See https://github.com/maxint-rd/
Partially based on OneButton library by Matthias Hertel. See https://github.com/mathertel/OneButton

*/


//#define TM16XX_DEBUG 1

#include "TM16xxButtons.h"

// constructor
TM16xxButtons::TM16xxButtons(TM16xx *pTM16xx, byte nMaxButtons) : _pTM16xx(pTM16xx), _nMaxButtons(nMaxButtons)
{ // Note: to avoid "warning: 'TM16xxButtons::_nMaxButtons' will be initialized after [-Wreorder]", parameters should be in same order as in class definition
	// TM16XX_OPT_BUTTONS_MALLOC: reduce memory by using dynamic memory allocation instead of static arrays for button states
	// requires additional constructor parameter to allow less than TM16XX_BUTTONS_MAXBUTTONSLOTS
#if(TM16XX_OPT_BUTTONS_MALLOC)
  _ButtonSlots=(TM16xxButtonSlot *)malloc(_nMaxButtons*sizeof(TM16xxButtonSlot));
#endif

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

#if(TM16XX_OPT_BUTTONS_EVENT)
// set function for release event
void TM16xxButtons::attachEventHandler(callbackTM16xxButtonsEvent newFunction)
{
  _eventFunc = newFunction;
} // attachEventHandler
#else
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
#endif

// For active buttons we use slots to track the state
// Using slots allows more buttons in less memory
byte TM16xxButtons::findSlot(byte nButton, byte nStateFind)
{
  for(byte n=0; n<_nMaxButtons; n++)
  {
    if(_ButtonSlots[n].button==nButton && (_ButtonSlots[n].state==nStateFind || nStateFind==TM16XX_BUTTONS_SLOT_ANYSTATE))
       { //Serial.print(F(".")); 
        return(n); }
    if(_ButtonSlots[n].button==nButton && nButton==TM16XX_BUTTONS_SLOT_UNUSED && (_ButtonSlots[n].state==nStateFind || nStateFind==TM16XX_BUTTONS_SLOT_ANYSTATE))
       { //Serial.print(F("^")); 
        return(n); }

    if(nButton==TM16XX_BUTTONS_SLOT_ANYBUTTON && (_ButtonSlots[n].state==nStateFind || nStateFind==TM16XX_BUTTONS_SLOT_ANYSTATE))
       { //Serial.print(F("~")); 
        return(n); }
       //return(n);
  }
  //Serial.print(F("*")); 
  return(TM16XX_BUTTONS_SLOT_NOTFOUND);
}


// function to get the current long pressed state
bool TM16xxButtons::isPressed(byte nButton)
{
	//if(nButton>=_nMaxButtons) return(false);
  nButton=findSlot(nButton);
  if(nButton==TM16XX_BUTTONS_SLOT_NOTFOUND) return(false);
  return(_ButtonSlots[nButton].state==TM16XX_BUTTONS_STATE_PRESSED || _ButtonSlots[nButton].state==TM16XX_BUTTONS_STATE_DBLPRESS || _ButtonSlots[nButton].state==TM16XX_BUTTONS_STATE_LPRESS);
}

// function to get the current long pressed state
bool TM16xxButtons::isLongPressed(byte nButton)
{
  nButton=findSlot(nButton);
  if(nButton==TM16XX_BUTTONS_SLOT_NOTFOUND) return(false);
	return(_ButtonSlots[nButton].state==TM16XX_BUTTONS_STATE_LPRESS);
}

int TM16xxButtons::getPressedTicks(byte nButton)
{
  nButton=findSlot(nButton);
  if(nButton==TM16XX_BUTTONS_SLOT_NOTFOUND) return(0);
  return((_ButtonSlots[nButton].stopTime - _ButtonSlots[nButton].startTime));		// uint16_t subtraction may overflow, but is still fine   0x01 - 0xFC = 0x05
}

void TM16xxButtons::reset(void)
{
  for(byte n=0; n<_nMaxButtons; n++)
  {
	  _ButtonSlots[n].button = TM16XX_BUTTONS_SLOT_UNUSED; // reset.
	  _ButtonSlots[n].state = TM16XX_BUTTONS_STATE_START; // restart.
	  _ButtonSlots[n].startTime = 0;
	  _ButtonSlots[n].stopTime = 0;
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
  Serial.print(F(", slots: "));
  for(byte n=0; n<_nMaxButtons; n++)
  {
    Serial.print(n);
    Serial.print(F("="));
    Serial.print(_ButtonSlots[n].button, HEX);
    Serial.print(F(", s:"));
    Serial.print(_ButtonSlots[n].state);
    Serial.print(F("    "));
  }
  // do a tick for each possible button
  for(byte n=0; n<32; n++)
  {
	  tick(n, dwButtons&bit(n));		// MMOLE 181103: _BV only works on 16-bit values!
	  //Serial.print(_state[n]);
	  //Serial.print(_state[n]);
	}
  for(byte n=0; n<_nMaxButtons; n++)
    Serial.print(_ButtonSlots[n].state);
//    Serial.print(_state[n]);
  //Serial.print(F("    "));
  Serial.println("");
#else
  // do a tick for each possible button
  for(byte n=0; n<32; n++)  // uint32_t dwButtons has 32 buttons at max 
	  tick(n, dwButtons&bit(n));		// MMOLE 181103: _BV only works on 16-bit values!
#endif
	return(dwButtons);
}

/**
 * @brief Advance the finite state machine (FSM) using the given level.
 */
void TM16xxButtons::tick(byte nButtonNum, bool activeLevel)
{
  uint16_t now = (uint16_t) millis(); // current (relative) time in msecs. To safe RAM we only use the bottom word (16 bits for instead of 32 for approx. 50 days)

  // find the slot of the button used, or prepare new slot
  byte nSlot=findSlot(nButtonNum);
  if(nSlot==TM16XX_BUTTONS_SLOT_NOTFOUND)
  { // no slot for same button, find a new one
    if(!activeLevel) return;    // new slots only get occupied when activated
//Serial.print(nButtonNum, HEX);
//Serial.print("@");
    nSlot=findSlot(TM16XX_BUTTONS_SLOT_UNUSED);
    if(nSlot==TM16XX_BUTTONS_SLOT_NOTFOUND)
    {   // no new slot, reuse any slot of state TM16XX_BUTTONS_STATE_START
//Serial.print("N");
      nSlot=findSlot(TM16XX_BUTTONS_SLOT_ANYBUTTON, TM16XX_BUTTONS_STATE_START);
      if(nSlot==TM16XX_BUTTONS_SLOT_NOTFOUND)
      {   // no more slots!
//Serial.print("!");
        return;
      }
      else
      { // found used slot in start state, reuse that slot
//Serial.print(nSlot, HEX);
//Serial.print("R");
        _ButtonSlots[nSlot].button=nButtonNum;
      }
    }
    else
    { // unused slot found, use it
//Serial.print(nSlot, HEX);
//Serial.print("U");
      _ButtonSlots[nSlot].button=nButtonNum;
    }
  }
  else
  {
//Serial.print(nSlot, HEX);
//Serial.print("F");
  }

  // Implementation of the state machine
  switch(_ButtonSlots[nSlot].state)
  {
  case TM16XX_BUTTONS_STATE_START:	// waiting for button being pressed.
    if (activeLevel)
    {
      _ButtonSlots[nSlot].state = TM16XX_BUTTONS_STATE_PRESSED; // step to pressed state
      _ButtonSlots[nSlot].startTime = now; // remember starting time
    } // if
    break;

  case TM16XX_BUTTONS_STATE_PRESSED: // waiting for button being released.
    if (!activeLevel)
    {
      _ButtonSlots[nSlot].state = TM16XX_BUTTONS_STATE_RELEASED; // step to released state
      _ButtonSlots[nSlot].stopTime = now; // remember stopping time
#if(TM16XX_OPT_BUTTONS_EVENT)
      if (_eventFunc)
        _eventFunc(TM16XX_BUTTONS_EVENT_RELEASE, nButtonNum);
#else
      if (_releaseFunc)
        _releaseFunc(nButtonNum);
#endif
    }
    else if ((activeLevel) && ((unsigned long)(now - _ButtonSlots[nSlot].startTime) > _longPressTicks))
    {
      _ButtonSlots[nSlot].state = TM16XX_BUTTONS_STATE_LPRESS; // step to long press state
      _ButtonSlots[nSlot].stopTime = now; // remember stopping time
#if(TM16XX_OPT_BUTTONS_EVENT)
      if (_eventFunc)
      {
        _eventFunc(TM16XX_BUTTONS_EVENT_LONGPRESSSTART, nButtonNum);
        _eventFunc(TM16XX_BUTTONS_EVENT_LONGPRESSBUSY, nButtonNum);
      }
#else
      if (_longPressStartFunc)
        _longPressStartFunc(nButtonNum);
      if (_duringLongPressFunc)
        _duringLongPressFunc(nButtonNum);
#endif
    } else {
      // wait. Stay in this state.
    } // if
    break;

  case TM16XX_BUTTONS_STATE_RELEASED: // waiting for button being pressed the second time or timeout.
#if(TM16XX_OPT_BUTTONS_EVENT)
    if ((unsigned long)(now - _ButtonSlots[nSlot].startTime) > _clickTicks)
#else
    if (_doubleClickFunc == NULL || (unsigned long)(now - _ButtonSlots[nSlot].startTime) > _clickTicks)
#endif
    {
      // this was only a single short click
#if(TM16XX_OPT_BUTTONS_EVENT)
      if (_eventFunc)
        _eventFunc(TM16XX_BUTTONS_EVENT_CLICK, nButtonNum);
#else
      if (_clickFunc)
        _clickFunc(nButtonNum);
#endif
      _ButtonSlots[nSlot].state = TM16XX_BUTTONS_STATE_START; // restart.
    }
    else if ((activeLevel))
    {
      _ButtonSlots[nSlot].state = TM16XX_BUTTONS_STATE_DBLPRESS; // step to doubleclick state
      _ButtonSlots[nSlot].startTime = now; // remember starting time
    } // if
    break;

  case TM16XX_BUTTONS_STATE_DBLPRESS: // waiting for button being released finally.
    if ((!activeLevel))
   	{
      // this was a 2 click sequence.
      _ButtonSlots[nSlot].state = TM16XX_BUTTONS_STATE_START; // restart.
      _ButtonSlots[nSlot].stopTime = now; // remember stopping time
#if(TM16XX_OPT_BUTTONS_EVENT)
      if (_eventFunc)
      {
        _eventFunc(TM16XX_BUTTONS_EVENT_RELEASE, nButtonNum);
        _eventFunc(TM16XX_BUTTONS_EVENT_DOUBLECLICK, nButtonNum);
      }
#else
      if (_releaseFunc)
        _releaseFunc(nButtonNum);
      if (_doubleClickFunc)
        _doubleClickFunc(nButtonNum);
#endif
    } // if
    break;

  case TM16XX_BUTTONS_STATE_LPRESS: // waiting for button being released after long press.
    if (!activeLevel)
    {
      _ButtonSlots[nSlot].state = TM16XX_BUTTONS_STATE_START; // restart.
      _ButtonSlots[nSlot].stopTime = now; // remember stopping time
#if(TM16XX_OPT_BUTTONS_EVENT)
      if (_eventFunc)
      {
        _eventFunc(TM16XX_BUTTONS_EVENT_RELEASE, nButtonNum);
        _eventFunc(TM16XX_BUTTONS_EVENT_LONGPRESSSTOP, nButtonNum);
      }
#else
      if (_releaseFunc)
        _releaseFunc(nButtonNum);
      if (_longPressStopFunc)
        _longPressStopFunc(nButtonNum);
#endif
    }
    else
    {
      // button is being long pressed
#if(TM16XX_OPT_BUTTONS_EVENT)
      if (_eventFunc)
        _eventFunc(TM16XX_BUTTONS_EVENT_LONGPRESSBUSY, nButtonNum);
#else
      if (_duringLongPressFunc)
        _duringLongPressFunc(nButtonNum);
#endif
    } // if
		break;
  } // switch
} // TM16xxButtons.tick(nButton)