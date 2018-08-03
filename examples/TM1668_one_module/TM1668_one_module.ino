/*
Library examples for TM1668.

Copyright (C) 2011 Ricardo Batista <rjbatista at gmail dot com>

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

#include <TM1668.h>

// define a module on data pin 8, clock pin 9 and strobe pin 7
TM1668 module(8, 9, 7);

void setup() {
  module.setupDisplay(true, 7);
  // display a hexadecimal number and set the left 4 dots
//  delay(1000);
  module.setDisplayToDecNumber(12345678, 0xF0);
  delay(1000);
//  module.setupDisplay(true, 7);
//  delay(1000);
  module.setupDisplay(true, 0);
/*
  delay(1000);
  module.setDisplayToHexNumber(0xABCD1234, 0xF0);
  delay(1000);
  for(int i=7; i>0; i--)
  {
    module.setupDisplay(true, i);
    delay(400);
  }
*/
  for(int i=0; i<6; i++)
  {
    module.sendChar(i, 0, false);
    delay(100);
  }
  for(int i=0; i<6; i++)
  {
    module.setupDisplay(true, i);
    for(int j=1; j<=0xFF; j=j*2+1)
    {
      module.sendChar(i, j, true);
      delay(50);
    }
  }
/**/
  for(int i=7; i>0; i--)
  {
    module.setupDisplay(true, i);
    delay(200);
  }
/**/
}

unsigned long nCnt=0;
void loop() {
  //  byte keys = module.getButtons();

  // light the first 4 red LEDs and the last 4 green LEDs as the buttons are pressed
  //  module.setLEDs(((keys & 0xF0) << 8) | (keys & 0xF));
  module.setDisplayToDecNumber(nCnt, 128);
  nCnt+=10;
  delay(10);
}
