/*
  Basic library example for 2 X TM1638 Common Anode up to 10 digit, 8 segment. 
  Kept small to show the simplest display functionality.
  Library based on TM1638 library by Ricardo Batista, adapted by Maxint-RD MMOLE 2018.
  
  Tested to work:
      ESP32 using Arduino IDE 1.8.13
  For more information see  https://github.com/maxint-rd/TM16xx
  Schemathics of Anode connection available here https://github.com/SkullKill/ESP32-Wall-Clock-PCB and https://github.com/SkullKill/ESP32-Wall-Clock-PCB/wiki

*/

#include <TM1638Anode.h>
#include <TM16xxDisplay.h>

//TM1638Anode module1(25, 26, 27, true, 7); // DIO=8, CLK=9, STB=7, activateDisplay, intensity
TM1638Anode module1(25, 26, 27);
TM16xxDisplay display1(&module1, 6);    // TM16xx object, 8 digits

//TM1638Anode module2(25, 26, 21, true, 1); // DIO=8, CLK=9, STB=7, activateDisplay, intensity
TM1638Anode module2(25, 26, 21);
TM16xxDisplay display2(&module2, 10);    // TM16xx object, 8 digits



void setup() {
//  display.println(F("HELLO !"));
  display1.println("12.34.5.6");
  display2.println("123456.7895");
  display2.setIntensity(2);
}

int nCount=0;
void loop() {
  delay(1000);
  display2.println((String)"Count:" + nCount);
  nCount++;
}
