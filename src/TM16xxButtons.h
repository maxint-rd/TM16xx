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

#define TM16XX_OPT_BUTTONS_EVENT 0			// use a single callback function instead of multiple (more flash, less heap)
#define TM16XX_OPT_BUTTONS_MALLOC 0			// use malloc to reserve button-state memory (more flash, less heap)

#ifndef TM16XX_BUTTONS_MAXBUTTONS
	#if defined(__AVR_ATtiny85__) ||  defined(__AVR_ATtiny13__) ||  defined(__AVR_ATtiny44__)
#define TM16XX_BUTTONS_MAXBUTTONS 8     // WARNING: changing this define outside of the header file requires recompilation of the library;
                                        // using without full recompile may cause very obscure crashes/resets
	#else
#define TM16XX_BUTTONS_MAXBUTTONS 32		// maximum number of buttons supported (determines heap used when not using malloc)
	#endif
#endif

#define TM16XX_BUTTONS_STATE_START 0
#define TM16XX_BUTTONS_STATE_PRESSED 1
#define TM16XX_BUTTONS_STATE_RELEASED 2
#define TM16XX_BUTTONS_STATE_DBLPRESS 3
#define TM16XX_BUTTONS_STATE_LPRESS 4


#if(TM16XX_OPT_BUTTONS_EVENT)
#define TM16XX_BUTTONS_EVENT_RELEASE 10
#define TM16XX_BUTTONS_EVENT_CLICK 20
#define TM16XX_BUTTONS_EVENT_DOUBLECLICK 30
#define TM16XX_BUTTONS_EVENT_LONGPRESSSTART 40
#define TM16XX_BUTTONS_EVENT_LONGPRESSSTOP 50
#define TM16XX_BUTTONS_EVENT_LONGPRESSBUSY 60
#endif

// ----- Callback function types -----

extern "C" {
typedef void (*callbackTM16xxButtons)(byte nButton);
#if(TM16XX_OPT_BUTTONS_EVENT)
typedef void (*callbackTM16xxButtonsEvent)(byte btEvent, byte nButton);
#endif
}

class TM16xxButtons
{
 public:
	TM16xxButtons(TM16xx *pTM16xx, byte nNumButtons=TM16XX_BUTTONS_MAXBUTTONS);


  // set # millisec after single click is assumed.
  void setClickTicks(int ticks);

  // set # millisec after long press is assumed.
  void setLongPressTicks(int ticks);

  // attach functions that will be called when button was pressed in the
  // specified way.
#if(TM16XX_OPT_BUTTONS_EVENT)
  void attachEventHandler(callbackTM16xxButtonsEvent newFunction);
#else
  void attachRelease(callbackTM16xxButtons newFunction);
  void attachClick(callbackTM16xxButtons newFunction);
  void attachDoubleClick(callbackTM16xxButtons newFunction);
  void attachLongPressStart(callbackTM16xxButtons newFunction);
  void attachLongPressStop(callbackTM16xxButtons newFunction);
  void attachDuringLongPress(callbackTM16xxButtons newFunction);
#endif
  uint32_t tick(void);
  /**
   * @brief Call this function every time the input level has changed.
   * Using this function no digital input pin is checked because the current
   * level is given by the parameter.
   */
  void tick(byte nButton, bool level);

  bool isPressed(byte nButton);
  bool isLongPressed(byte nButton);
  int getPressedTicks(byte nButton);
  void reset(void);

 protected:
  TM16xx *_pTM16xx;

 private:
 	byte _nNumButtons;
  unsigned int _clickTicks = 500; // number of ticks that have to pass by
                                  // before a click is detected.
  unsigned int _longPressTicks = 1000; // number of ticks that have to pass by
                                   // before a long button press is detected

  // These variables will hold functions acting as event source.
#if(TM16XX_OPT_BUTTONS_EVENT)
  callbackTM16xxButtonsEvent _eventFunc = NULL;
#else
  callbackTM16xxButtons _releaseFunc = NULL;
  callbackTM16xxButtons _clickFunc = NULL;
  callbackTM16xxButtons _doubleClickFunc = NULL;
  callbackTM16xxButtons _longPressStartFunc = NULL;
  callbackTM16xxButtons _longPressStopFunc = NULL;
  callbackTM16xxButtons _duringLongPressFunc = NULL;
#endif

  // These variables that hold information across the upcoming tick calls.
  // They are initialized once on program start and are updated every time the
  // tick function is called.
#if(TM16XX_OPT_BUTTONS_MALLOC)
  byte *_state;		// allocated memory array
  unsigned long *_startTime; 		// allocated memory array, value is set in state TM16XX_BUTTONS_STATE_PRESSED
  unsigned long *_stopTime; 		// allocated memory array, value is set in state TM16XX_BUTTONS_STATE_RELEASED
#else
  byte _state[TM16XX_BUTTONS_MAXBUTTONS];			// = TM16XX_BUTTONS_STATE_START;
  //unsigned long _startTime[TM16XX_BUTTONS_MAXBUTTONS]; // will be set in state TM16XX_BUTTONS_STATE_PRESSED
  //unsigned long _stopTime[TM16XX_BUTTONS_MAXBUTTONS]; // will be set in state TM16XX_BUTTONS_STATE_RELEASED
  uint16_t _startTime[TM16XX_BUTTONS_MAXBUTTONS]; // will be set in state TM16XX_BUTTONS_STATE_PRESSED
  uint16_t _stopTime[TM16XX_BUTTONS_MAXBUTTONS]; // will be set in state TM16XX_BUTTONS_STATE_RELEASED
#endif
};
#endif