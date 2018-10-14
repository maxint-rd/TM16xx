/*
  TM16xx Library example to show advanced button functionality.

  This example is for TM1638 LED&KEY module but can also be used for the extended TM1637 mini LED&KEY module.
  The extended TM1637 module has 4 push-buttons (KS1-KS4 to K2) and 4 bi-color leds (SEG1-SEG4 to GRID5/GRID6)

  It was tested on Arduino Nano (old bootloader) in Arduino IDE 1.8.2. (7982B flash, 688B RAM)

  Made by Maxint-RD MMOLE 2018. see GitHub.com/maxint-rd/TM16xx
*/
//#include <TM1637.h>
#include <TM1638.h>
#include <TM16xxDisplay.h>
#include <TM16xxButtons.h>

// Define the module on data pin 8, clock pin 9 and strobe pin 7
// Define the buttons object for detecting button states
// also define a display object for using print()
//TM1637 module(5, 4);  // DIO=5, CLK=4
TM1638 module(8, 9, 7);   // DIO=8, CLK=9, STB=7
TM16xxButtons buttons(&module);       // TM16xx button 
TM16xxDisplay display(&module, 8);    // TM16xx object, 8 digits

// function to show some indicative blinks
void Blink(int nDelay=500, int nTimes=1)
{
  for(int i=0; i<nTimes; i++)
  {
  digitalWrite(LED_BUILTIN, HIGH);    // switch on (or active low LED off for ESP)
  delay(nDelay);
  digitalWrite(LED_BUILTIN, LOW);    // switch off (or active low LED on for ESP)
  delay(nDelay);
  }
}


void setup()
{
  Serial.begin(115200);
  Serial.println(F("TM16xxButtons example"));
  Serial.println(F("TM1638: DIO=8, CLK=9, STB=7"));
  //Serial.println(F("TM1637: DIO=5, CLK=4"));
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.println();

  Serial.println(F("clear"));
  digitalWrite(LED_BUILTIN, LOW);     // switch (active) low LED off
  module.clearDisplay();              // clear display
  module.setupDisplay(true, 7);       // set intensity 0-7, 7=highest
  delay(50);

  // show some text
  Serial.println(F("txt"));
  module.setDisplayToString("HALO");
  delay(400);
  module.setDisplayToString("YOUTUBE ");
  delay(400);

  // link the button callback functions.
  buttons.attachClick(fnClick);
  buttons.attachDoubleClick(fnDoubleclick);
  buttons.attachLongPressStart(fnLongPressStart);
  buttons.attachLongPressStop(fnLongPressStop);
  buttons.attachDuringLongPress(fnLongPress);

  Serial.println(F("clear"));
  module.clearDisplay();
  Serial.println(F("Setup done"));
}

void loop()
{
  char text[17];
  uint32_t dwButtons=buttons.tick();

  //sprintf(text, "%lu", millis());
  ltoa(millis(), text, 10);   // DECIMAL = base 10

  if(dwButtons)
    display.setDisplayToHexNumber(dwButtons, 0, false);
  else
    module.setDisplayToString(text);

  delay(100);
}


//
// Button callback functions
//

// This function will be called when a button was pressed 1 time (without a second press).
void fnClick(byte nButton)
{
  Serial.print(F("Button "));
  Serial.print(nButton);
  Serial.println(F(" click."));
  display.println(F("sclk  "));
  Blink(100,1);
} // click


// This function will be called when a button was pressed 2 times in a short timeframe.
void fnDoubleclick(byte nButton)
{
  Serial.print(F("Button "));
  Serial.print(nButton);
  Serial.println(F(" doubleclick."));
  display.println(F("dclk  "));
  Blink(200,2);
} // doubleclick


// This function will be called once, when a button is pressed for a long time.
void fnLongPressStart(byte nButton)
{
  Serial.print(F("Button "));
  Serial.print(nButton);
  Serial.println(F(" longPress start"));
  display.println(F("strt  "));
  Blink(50,1);
} // longPressStart


// This function will be called often, while a button is pressed for a long time.
void fnLongPress(byte nButton)
{
  Serial.print(F("Button "));
  Serial.print(nButton);
  Serial.println(F(" longPress..."));
} // longPress


// This function will be called once, when a button is released after beeing pressed for a long time.
void fnLongPressStop(byte nButton)
{
  Serial.print(F("Button "));
  Serial.print(nButton);
  Serial.println(F(" longPress stop"));
  display.println(F("stop  "));
  Blink(500,1);
} // longPressStop
