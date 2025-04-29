/*
TM1638.cpp - Library implementation for TM1638.

Copyright (C) 2011 Ricardo Batista (rjbatista <at> gmail <dot> com)
Rewrite for TM16xx library by Maxint R&D. See https://github.com/maxint-rd/

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "TM1638.h"

TM1638::TM1638(byte dataPin, byte clockPin, byte strobePin, byte numDigits, bool activateDisplay, byte intensity)
  : TM16xx(dataPin, clockPin, strobePin, TM1638_MAX_POS, numDigits, activateDisplay, intensity)
{
  // DEPRECATED: activation, intensity (0-7) and display mode are no longer used by constructor.  
  _maxSegments=10;    // on the LED & KEY modules the extra segments are used to drive individual red or red/green LEDs
/*
  clearDisplay();
  setupDisplay(activateDisplay, intensity);
*/
  // NOTE: CONSTRUCTORS SHOULD NOT CALL DELAY() <= gives hanging on certain ESP8266 cores as well as on LGT8F328P
  // Using micros() or millis() in constructor also gave issues on LST8F328P and CH32.
  // Root cause is that the timer interrupt to update these counters may not be set until setup() is called.
  // To initialize the display an implicit begin() is called by TM16xx::sendData().
}

void TM1638::setSegments(byte segments, byte position)
{ // set 8 leds on common grd as specified
  // TODO: support 10-14 segments on chips like TM1638/TM1668
  // TM1638 uses 10 segments in two bytes, similar to TM1668
  // for the digit displays only the first byte (containing seg1-seg8) is sent
  // Only the LSB (SEG1-8) is sent to the display
  if(position<_maxDisplays)
    sendData(position << 1, segments);
}

void TM1638::setSegments16(uint16_t segments, byte position)
{ // method to send more than 8 segments (10 max for TM1638)
  // TM1638 uses 10 segments in two bytes, similar to TM1668
  // segments 0-7 are in bits 0-7 of position bytes 0,2,4,6,8,10,12,14
  // segments 8-9 are in bits 0-1 of position bytes 1,3,5,7,9,11,13,15
  if(position<_maxDisplays)
  {
    sendData(position << 1, (byte)segments&0xFF);
    sendData((position << 1) | 1, (byte)(segments>>8)&0x03);
  }
}

void TM1638::setLED(byte color, byte pos)
{ // TM1638 uses 10 segments in two bytes, for the LEDs only the second byte (containing seg9-seg10) is send
  sendData((pos << 1) + 1, color);
}

void TM1638::setLEDs(word leds)
{
  for (int i = 0; i < digits; i++) {
    byte color = 0;

    if ((leds & (1 << i)) != 0) {
      color |= TM1638_COLOR_RED;
    }

    if ((leds & (1 << (i + 8))) != 0) {
      color |= TM1638_COLOR_GREEN;
    }

    setLED(color, i);
  }
}

uint32_t TM1638::getButtons(void)
{
  // TM1638 returns 4 bytes/8 nibbles for keyscan. Each byte has K3, K2 and K1 status in lower bits of each nibble for KS1-KS8
  // NOTE: K1/K2 are implemented for this class, but the TM1638 LED&KEY module only uses K3.
  //       Also note that the buttons are wired in odd sequence: S1=KS1, S5=KS2, S2=KS3, S6=KS4, S3=KS5, S7=KS6, S4=KS7, S8=KS8
  byte keys_K1 = 0;
  byte keys_K2 = 0;
  byte keys_K3 = 0;

  start();
  send(TM16XX_CMD_DATA_READ); // B01000010 Read the key scan data

  // According datasheet TM1638 v2.2 p.9 the maximum speed for CLK is 1MHz, but when reading, there needs to be a Twait 
  // of minimum 2us between the read command and the first receive. bitDelay() is set to 1us on faster processors. 
  bitDelay();   // extra delay for the read command.
  
  for (int i = 0; i < 4; i++)
  {
    byte rec = receive();
    keys_K1 |= ((rec&_BV(2))>>2 | (rec&_BV(6))>>5) << (2*i);      // bits 2 and 6 for K1/KS1 and K1/KS2
    keys_K2 |= ((rec&_BV(1))>>1 | (rec&_BV(5))>>4) << (2*i);      // bits 1 and 5 for K2/KS1 and K2/KS2
    keys_K3 |= ((rec&_BV(0))    | (rec&_BV(4))>>3) << (2*i);      // bits 0 and 4 for K3/KS1 and K3/KS2
  }

  stop();

  return((uint32_t)keys_K3<<16 | (uint32_t)keys_K2<<8 | (uint32_t)keys_K1);
}