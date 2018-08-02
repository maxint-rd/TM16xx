/*
Library examples for TM1640.

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
//#include <TM1638.h> // MMOLE:isn't required in Arduino IDE1.6.10, was required because the way arduino deals with libraries
#include <TM1637.h>

// define a module on data pin 5 (D1), clock pin 4 (D2)
TM1637 module(5, 4);

void setup()
{
  Serial.begin(115200);
  Serial.println("Setup start");
  Serial.println();
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(10);

  Serial.println("clear");
  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  module.clearDisplay();
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(50);

  Serial.println("txt");
  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  module.setDisplayToString("8888", 0xFF);
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(10);

  // dim the display
  Serial.println("dim");
  for(int j=0; j<3; j++)
  {
    for(int i=7; i>0; i--)
    {
      module.dimDisplay(true, i);
      delay(100);
    }
    for(int i=0; i<8; i++)
    {
      module.dimDisplay(true, i);
      delay(100);
    }
  }

  // set some ledsegments
  Serial.println("ledsegments");
  for(int j=4; j<TM1637_MAX_POS; j++)
  {
    for(int i=0; i<=0x0F; i++)
    {
      module.setSegments(i, j);
      delay(100);
      module.setSegments(0, j);
    }
    module.setSegments(0, j);
  }


  Serial.println("clear");
  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  module.clearDisplay();
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(50);


  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  delay(1);
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(100);

  Serial.println("Setup done");
}

byte btLeds=0;
byte btPosition=0;
void loop()
{
  char text[17];

  //sprintf(text, "testing %u", millis());
  sprintf(text, "%u", millis());
  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));     // blink the LED

  module.setDisplayToString(text);
  byte btButtons=module.getButtons();
  Serial.print("Buttons: ");
  Serial.println(btButtons, HEX);

  if(btButtons)
  {
    if(btLeds&_BV(btButtons-1)) btPosition++;
    if(btPosition>1) btPosition=0;
    btLeds= _BV(btButtons-1);
    module.setSegments(btLeds, btPosition+4);
  }

  delay(100);
}
