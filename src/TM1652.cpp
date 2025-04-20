/*
TM1652.h - Library for TM1652 led display driver. Only DIN, 8x5/7x6 LED, no buttons.

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 as well as individual chips.
Simply use print() on 7-segment displays and use Adafruit GFX on matrix displays.

Made by Maxint R&D. See https://github.com/maxint-rd
*/

#include "TM1652.h"

TM1652::TM1652(byte dataPin, byte numDigits, bool activateDisplay, byte intensity,  byte displaymode)
	: TM16xx(dataPin, dataPin, dataPin, TM1652_MAX_POS, numDigits, activateDisplay, intensity)
{ // The TM1652 only has DIN, no CLK or STB. Therefor the DIN-pin is initialized in the parent-constructor as CLK and STB, but not used for anything else.
  // DEPRECATED: activation, intensity (0-7) and display mode are no longer used by constructor.  
  // For TM1652 the display mode is either 5x8 or 6x7. setupDisplay() will set display mode to 7 segments when numDigits is larger than 5.

  // NOTE: CONSTRUCTORS SHOULD NOT CALL DELAY() <= gives hanging on certain ESP8266 cores as well as on LGT8F328P
  // Using micros() or millis() in constructor also gave issues on LST8F328P and CH32.
  // Root cause is that the timer interrupt to update these counters may not be set until setup() is called.

  // TM1652 uses bit timing to communicate, so clearDisplay() and setupDisplay() cannot be called in constructor.
  // Call begin() in setup() to clear the display and set initial activation and intensity.

	// Actual setting of display mode is chip is done in setupDisplay(), which also sets intensity and on/off state
  _maxSegments = (numDigits>5?7:8);    // default display mode: 5 Grid x 8 Segments (TM1652: 5x8 or 6x7)
}
/*
TODO: remove deprecated parameters
TM1652::TM1652(byte dataPin, byte numDigits)
	: TM16xx(dataPin, dataPin, dataPin, TM1652_MAX_POS, numDigits, true, 7)
*/

void TM1652::begin(bool activateDisplay, byte intensity, byte driveCurrent)
{ // Call begin() in setup() to clear the display and set initial activation and intensity.
  // begin() is implicitly called upon first sending of display data, but only executes once.
  if(fBeginDone)
    return;
  fBeginDone=true;
  clearDisplay();
  setupDisplay(activateDisplay, intensity, driveCurrent);
} 
  
void TM1652::start()
{ // For the TM1652, start and stop are sent using serial UART protocol so no separate start or stop
}

void TM1652::stop()
{ // For the TM1652, start and stop are sent using serial UART protocol so no separate start or stop
}

void TM1652::send(byte data)
{	// Send a byte to the chip the way the TM1652 likes it (LSB-first, UART serial 8E1 - 8 bits, parity bit set to 0 when odd, one stop bit)
	// Note: while segment data is LSB-first, address bits and SEG/GRID intensity bit are reversed
	// The address command can be followed by multiple databytes, requiring specific timing to distinguish multiple commands
  //  - start bit, 8x data bits, parity bit, stop bit; 52 us = 19200bps
  // (Datasheet: "The baud rate range supported by TM1652 is: 17500bps ~ 21200bps [...] the time range of each bit is: 47us ~ 57us."
  // To compensate for processing instructions on slower processors, BITDELAY is set lower than 52
  // Testing on CH32V003 @ 48Mhz, showed that value working fine. Faster processors may need a higher delay. TODO: test other MCU's.
  #define TM1652_BITDELAY 49     // NOTE: core 1.0.6 of LGT8F328@32MHz miscalculates delayMicroseconds() (should be 52us delay). For fix see https://github.com/dbuezas/lgt8fx/issues/18
  bool fParity=true;

  // Note: To improve timing accuracy, sending data should not be interrupted. 
  // However, having interrupts may be required by timing functions such as delayMicroseconds (e.g. on RP2040)
  // Interrupts during data could be a bigger issue on slower processors.

  // start - low
  digitalWrite(dataPin, LOW);
  delayMicroseconds(TM1652_BITDELAY);

  // data low-0; high=1
  for(int nBit=0; nBit<8; nBit++)
  {
    if(data&1) fParity=!fParity;
    digitalWrite(dataPin, (data&1) ? HIGH : LOW);
    data>>=1;
    delayMicroseconds(TM1652_BITDELAY);
  }

  // parity - low when odd
  digitalWrite(dataPin, fParity);
  delayMicroseconds(TM1652_BITDELAY);

  // stop - high
  digitalWrite(dataPin, HIGH);
  delayMicroseconds(TM1652_BITDELAY);

  // idle - remain high
  delayMicroseconds(TM1652_BITDELAY);
}

void TM1652::waitCmd(void)
{ // Wait for at least 3ms since previous call to ensure TM1652 treats next byte as new command
  // Datasheet: "Time: Data line high time (minimum time is 3ms)"
  #define TM1652_WAITCMD 3000L
  while(micros() < tLastCmd + TM1652_WAITCMD);    // NOTE: tLastCmd is 32-bit => wrap-around is about 4295 sec
}

void TM1652::endCmd(void)
{ // signal the end of a command, to remember the timing
  tLastCmd=micros();
}

void TM1652::sendData(byte address, byte data)
{	// TM1652 uses different commands than other TM16XX chips
  //   byte positions[]={B00001000, B10001000, B01001000, B11001000, B00101000, B10101000};
  begin();    // begin() is implicitly called upon first sending of display data, but only executes once.
  waitCmd();
  send(TM1652_CMD_ADDRESS | reverseByte(address&0x07));						// address command + address (reversed as documented)
  send(data);
  endCmd();
}

void TM1652::sendCommand(byte cmd)
{ // send a display command
  waitCmd();
  send(TM1652_CMD_MODE);
  send(cmd);
  endCmd();
}

void TM1652::clearDisplay()
{	// Clear all data registers. The number of registers depends on the chip.
	// TM1652 (8x5/7x6): 8 or 7 segments per grid, stored in one byte. 
  waitCmd();
	send(TM1652_CMD_ADDRESS);
  for (int i = 0; i < _maxDisplays; i++) {
  	send(0);
  }
  endCmd();
}

void TM1652::setupDisplay(bool active, byte intensity, byte driveCurrent)
{	// Set the display active/inactive, set display mode and set intensity/driveCurrent.
  // For the TM1652 the levels 0-7 are low to high.
	// In addition to setting drive current in 8 steps, TM1652 also allows setting the duty cycle in 16 steps.
	// To align with other TM16XX chips we translate intensity to the same comparible scale (0-7) to set the duty cycle.
	// Drive current can be set on a scale from (0-6). There are 8 levels, but only 7 are usable.
  // Datasheet: "When the segment drive current is 1/8, the current provided by the chip is not enough to light
  //             the ordinary digital tube, so it is recommended to set the segment drive current to 2/8 at least".
	// sendCommand(0xEE);
  // sendCommand((active ? 0xF0 : 0) | reverseByte((intensity&0x07)<<4) | (_maxSegments==8? TM1652_DISPMODE_5x8 : TM1652_DISPMODE_6x7));
  sendCommand((active ? reverseByte(((intensity&0x07)<<1)|0x01) : 0) | (active ? (reverseByte((driveCurrent + 0x01)&0x07) >> 4) : 0) | (_maxSegments==8? 0 : 1));
}

byte TM1652::reverseByte(byte b)
{ // Reverse the bits in a byte LSB<->MSB
  // See https://stackoverflow.com/questions/2602823/in-c-c-whats-the-simplest-way-to-reverse-the-order-of-bits-in-a-byte
  b = (b & 0b11110000) >> 4 | (b & 0b00001111) << 4;
  b = (b & 0b11001100) >> 2 | (b & 0b00110011) << 2;
  b = (b & 0b10101010) >> 1 | (b & 0b01010101) << 1;
  return b;
}
