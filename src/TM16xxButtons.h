/*

TM16xxButtons.h - Buttons class for TM16xx.
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
#ifndef _TM16XX_BUTTONS_H
#define _TM16XX_BUTTONS_H

#include "TM16xx.h"

#define TM16XX_BUTTONS_MAXBUTTONS 32
#define TM16XX_BUTTONS_STATE_START 0
#define TM16XX_BUTTONS_STATE_PRESSED 1
#define TM16XX_BUTTONS_STATE_RELEASED 2
#define TM16XX_BUTTONS_STATE_DLPRESS 3
#define TM16XX_BUTTONS_STATE_LPRESS 6

// ----- Callback function types -----

extern "C" {
typedef void (*callbackTM16xxButtons)(byte nButton);
}


class TM16xxButtons
{
 public:
	TM16xxButtons(TM16xx *pTM16xx);


  // set # millisec after single click is assumed.
  void setClickTicks(int ticks);

  // set # millisec after press is assumed.
  void setPressTicks(int ticks);

  // attach functions that will be called when button was pressed in the
  // specified way.
  void attachClick(callbackTM16xxButtons newFunction);
  void attachDoubleClick(callbackTM16xxButtons newFunction);
  void attachLongPressStart(callbackTM16xxButtons newFunction);
  void attachLongPressStop(callbackTM16xxButtons newFunction);
  void attachDuringLongPress(callbackTM16xxButtons newFunction);


  uint32_t tick(void);
  /**
   * @brief Call this function every time the input level has changed.
   * Using this function no digital input pin is checked because the current
   * level is given by the parameter.
   */
  void tick(byte nButton, bool level);

  bool isLongPressed(byte nButton);
  int getPressedTicks(byte nButton);
  void reset(void);

 protected:
  TM16xx *_pTM16xx;

 private:
  unsigned int _clickTicks = 500; // number of ticks that have to pass by
                                  // before a click is detected.
  unsigned int _pressTicks = 1000; // number of ticks that have to pass by
                                   // before a long button press is detected
  int _buttonPressed;
  bool _isLongPressed[TM16XX_BUTTONS_MAXBUTTONS];		// = false;

  // These variables will hold functions acting as event source.
  callbackTM16xxButtons _clickFunc = NULL;
  callbackTM16xxButtons _doubleClickFunc = NULL;
  callbackTM16xxButtons _longPressStartFunc = NULL;
  callbackTM16xxButtons _longPressStopFunc = NULL;
  callbackTM16xxButtons _duringLongPressFunc = NULL;

  // These variables that hold information across the upcoming tick calls.
  // They are initialized once on program start and are updated every time the
  // tick function is called.
  int _state[TM16XX_BUTTONS_MAXBUTTONS];			// = 0;
  unsigned long _startTime[TM16XX_BUTTONS_MAXBUTTONS]; // will be set in state 1
  unsigned long _stopTime[TM16XX_BUTTONS_MAXBUTTONS]; // will be set in state 2

};
#endif