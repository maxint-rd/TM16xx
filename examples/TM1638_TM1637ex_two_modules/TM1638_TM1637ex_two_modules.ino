/*
Library example for TM1637+TM1638.
This example is for the extended TM1637 4-digit LED display module in combination with the TM1638 LED&KEY module.
It was tested on Arduino Nano (old bootloader) in Arduino IDE 1.8.2. 6008 bytes flash, 484 bytes RAM. ==> 5772/478 => 6294/490 ==> 6366/368 ==> 6526/372
The extended module has 4 push-buttons (KS1-KS4 to K2) and 4 bi-color leds (SEG1-SEG4 to GRID5/GRID6)

Based on TM1640 example by Ricardo Batista, adapted by Maxint-RD MMOLE 2018
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
#include <TM1637.h>
#include <TM1638.h>
#include <TM16xxDisplay.h>

// define a module on data pin 8, clock pin 9 and strobe pin 7
TM1638 module38(8, 9, 7);   // DIO=8, CLK=9, STB=7
TM16xxDisplay display38(&module38, 8);    // TM16xx object, 8 digits

// define a module on data pin 5 (D1), clock pin 4 (D2)
TM1637 module37(5, 4);  // DIO=5, CLK=4
TM16xxDisplay display37(&module37, 4);    // TM16xx object, 4 digits

void setup()
{
  Serial.begin(115200);
  Serial.println(F("TM1638/TM1637 example"));
  Serial.println(F("TM1638: DIO=8, CLK=9, STB=7"));
  Serial.println(F("TM1637: DIO=5, CLK=4"));
  Serial.println();
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(10);

/**/     
  Serial.println(F("clear"));
  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  module37.clearDisplay();
  module37.setupDisplay(true, 7);
  module38.clearDisplay();
  //module38.setupDisplay(true, 7);
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(50);
/**/

  Serial.println(F("txt"));
  module37.setDisplayToString("HALO");
  module38.setDisplayToString("HALO");
  delay(400);
  module38.setDisplayToString("YOUTUBE ");
  module37.setDisplayToString("YOU ");
  delay(300);
  module37.setDisplayToString("TUBE");
  delay(400);

  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  module37.setDisplayToString("8888", 0xFF);
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(10);

  Serial.println(F("print"));
  display38.print(F("print: "));
  display38.println(123456, DEC);
  delay(500);
  display38.println(1234.5678);
  delay(500);
  display38.println(0xDEADBEEF, HEX);
  delay(500);
  display38.println("a.b.c:d,e;fg!");

  // dim the display
  Serial.println(F("dim"));
  for(int j=0; j<3; j++)
  {
    display37.print("Dm.");
    display37.println(j+1);
    for(int i=7; i>0; i--)
    {
      module37.setupDisplay(true, i);
      module38.setupDisplay(true, i);
      delay(100);
    }
    for(int i=0; i<8; i++)
    {
      module37.setupDisplay(true, i);
      module38.setupDisplay(true, i);
      delay(100);
    }
  }

  // set some ledsegments to light up the extra leds
  Serial.println(F("ledsegments"));
  for(int j=4; j<TM1637_MAX_POS; j++)
  {
    for(int i=0; i<=0x0F; i++)
    {
      display37.print("led");
      display37.println(i, HEX);
      module37.setSegments(i, j);
      delay(100);
      module37.setSegments(0, j);
    }
    module37.setSegments(0, j);
  }


  Serial.println(F("clear"));
  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  module37.clearDisplay();
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(50);


  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  delay(1);
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(100);

  module37.clearDisplay();
  module38.clearDisplay();
  Serial.println(F("Setup done"));
}

byte btLeds=0;
byte btPosition=0;
void loop()
{
  char text[17];

  //sprintf(text, "%lu", millis());
  ltoa(millis(), text, 10);   // DECIMAL = base 10

  byte btButtons=module37.getButtons()&0x0F;    // only need bits 0-3
  Serial.print(F("Buttons: "));
  Serial.print(btButtons, HEX);
  uint32_t btButtons2=module38.getButtons();
  Serial.print(F(",  "));
  Serial.println(btButtons2, HEX);

  if(btButtons)
  {
    if(btLeds&btButtons) btPosition=1-btPosition;
    btLeds=btButtons;
    module37.setSegments(btLeds, btPosition+4);  // expanded TM1637 has 4 RG-LEDs on SEG1-4 of GRD5/GRD6
  }

  if(btButtons || btButtons2)
  {
    display37.setDisplayToHexNumber((btButtons<<8)+btButtons2, 0, false);
    display38.setDisplayToHexNumber((btButtons<<8)+btButtons2, 0, false);
  }
  else
  {
    module37.setDisplayToString(text);
    module38.setDisplayToString(text);
  }
  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));     // blink the LED

  delay(100);
}
