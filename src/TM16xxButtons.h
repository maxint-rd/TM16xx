/*

TM16xxButtons.h - Buttons class for TM16xx.
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

#ifndef _TM16XX_BUTTONS_H
#define _TM16XX_BUTTONS_H

#include "TM16xx.h"

#define TM16XX_OPT_BUTTONS_EVENT 0			// use a single callback function instead of multiple (more flash, less heap)

//
// NOTE: Each button-slot uses 6 bytes to store button-state and timings.
//       To minimize RAM or FLASH memory used, the number of button slots and the method of memory allocation can be set here.
//       Your usage determines what setting is best. If you're low on RAM, or your module has few buttons or only supports
//       single presses, you can set the number of slots to minimum (eg. 1 or 2).
// EXPERIMENTAL:
//       If you want to try, malloc may be used, but beware of leakage.
//       This implementation is experimental and there is no destructor (yet) to free the memory. 
//
#define TM16XX_OPT_BUTTONS_MALLOC 0			// 1=use malloc to reserve button-state memory (much more flash but less heap and dynamic)
#define TM16XX_BUTTONS_MAXBUTTONS 32		// maximum number of buttons supported is depending on the chip used, but max 32 bits are used in the base library
#ifndef TM16XX_BUTTONS_MAXBUTTONSLOTS   // button slots are used to track button states, TM1637/TM1650 don't support combined presses
  #if defined(__AVR_ATtiny85__) ||  defined(__AVR_ATtiny45__) ||  defined(__AVR_ATtiny13__) ||  defined(__AVR_ATtiny44__) ||  defined(__AVR_ATtiny84__) // NOTE: ATtiny13 is really too tiny for this.
    #define TM16XX_BUTTONS_MAXBUTTONSLOTS 2     // WARNING: changing this define outside of the header file requires recompilation of the library;
                                                // using without full recompile may cause very obscure crashes/resets
  #else
    #define TM16XX_BUTTONS_MAXBUTTONSLOTS 4		// maximum number of buttonslots active (determines heap used when not using malloc)
  #endif
#endif


#define TM16XX_BUTTONS_SLOT_UNUSED 0xFF
#define TM16XX_BUTTONS_SLOT_NOTFOUND 0xFF
#define TM16XX_BUTTONS_SLOT_ANYSTATE 0xFF
#define TM16XX_BUTTONS_SLOT_ANYBUTTON 0xFE

#define TM16XX_BUTTONS_STATE_START 0          // button not pressed
#define TM16XX_BUTTONS_STATE_PRESSED 1        // button pressed
#define TM16XX_BUTTONS_STATE_RELEASED 2       // button released
#define TM16XX_BUTTONS_STATE_DBLPRESS 3       // button double press
#define TM16XX_BUTTONS_STATE_LPRESS 4         // button long pressed


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

struct TM16xxButtonSlot
{
  byte button;          // number of the button in the slot (0-31 or TM16XX_BUTTONS_SLOT_UNUSED)
  byte state;			      // current state of the button tracked, initially TM16XX_BUTTONS_STATE_START;
  uint16_t startTime;   // time started; will be set in state TM16XX_BUTTONS_STATE_PRESSED
  uint16_t stopTime;    // time stopped; will be set in state TM16XX_BUTTONS_STATE_RELEASED
};

class TM16xxButtons
{
 public:
	TM16xxButtons(TM16xx *pTM16xx, byte nMaxButtons=TM16XX_BUTTONS_MAXBUTTONSLOTS);


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
  byte _nMaxButtons;    // maximum number of buttons tracked (can be less than TM16XX_BUTTONS_MAXBUTTONSLOTS when using malloc)
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
  TM16xxButtonSlot *_ButtonSlots;
#else
  TM16xxButtonSlot _ButtonSlots[TM16XX_BUTTONS_MAXBUTTONSLOTS];
#endif
  byte findSlot(byte nButton, byte nStateFind=TM16XX_BUTTONS_SLOT_ANYSTATE);
};
#endif