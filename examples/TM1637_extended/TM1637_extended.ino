/*
  Library example for TM1637.
  This example is for the extended TM1637 4-digit LED display module.
  It was tested on ESP8266 in Arduino IDE 1.6.10
  It was tested on Arduino Nano in Arduino IDE 1.8.2
  The extended module has 4 push-buttons (KS1-KS4 to K2) and 4 bi-color leds (SEG1-SEG4 to GRID5/GRID6)

  Tested to work:
      Arduino Nano using Arduino IDE 1.8.2, Nano (Old Bootloader)), 6368 bytes flash, 306 bytes RAM

  Based on TM1640 example by Ricardo Batista, adapted by Maxint-RD MMOLE 2018
  For more information see  https://github.com/maxint-rd/TM16xx
*/
#include <TM1637.h>

// define a module on data pin 5 (D1), clock pin 4 (D2)
TM1637 module(5, 4);  // DIO=5, CLK=4

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Setup start"));
  Serial.println();
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(10);

  Serial.println(F("clear"));
  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  module.clearDisplay();
  delay(1);
  module.setupDisplay(true, 7);
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(50);

  Serial.println(F("txt"));
  module.setDisplayToString("HALO");
  delay(400);
  module.setDisplayToString("YOU ");
  delay(300);
  module.setDisplayToString("TUBE");
  delay(400);

  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  module.setDisplayToString("8888", 0xFF);
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(10);

  // dim the display
  Serial.println(F("dim"));
  for(int j=0; j<3; j++)
  {
    for(int i=7; i>0; i--)
    {
      module.setupDisplay(true, i);
      delay(100);
    }
    for(int i=0; i<8; i++)
    {
      module.setupDisplay(true, i);
      delay(100);
    }
  }

  // set some ledsegments
  Serial.println(F("ledsegments"));
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


  Serial.println(F("clear"));
  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  module.clearDisplay();
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(50);


  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  delay(1);
  digitalWrite(LED_BUILTIN, HIGH);     // switch (active) low LED off
  delay(100);

  Serial.println(F("Setup done"));
}

byte btLeds=0;
byte btPosition=0;
void loop()
{
  char text[17];

  sprintf(text, "%u", millis());
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));     // blink the LED

  module.setDisplayToString(text);
  byte btButtons=module.getButtons()&0x0F;    // only need bits 0-3
  Serial.print("Buttons: ");
  Serial.println(btButtons, HEX);

  if(btButtons)
  {
    if(btLeds&btButtons) btPosition=1-btPosition;
    btLeds=btButtons;
    module.setSegments(btLeds, btPosition+4);
  }

  delay(100);
}
