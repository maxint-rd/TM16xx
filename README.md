# TM16xx
TM16xx library for Arduino. Supports LED and KEY modules based on TM1638 and similar chips.

## TM16xx LEDs and Buttons library
A library for interacting an Arduino with a TM1637/TM1638/TM1640/TM1668.
The TMxx chip family allows driving 7-segment LED displays or LED matrices.
Most TMxx chips also support reading key-scan data.
Next to built-in high-frequency LED multiplexing, they offer control of LED brightness.

Made by Maxint R&D. See https://github.com/maxint-rd/
Based on TM1638 library by Ricardo Batista, see https://github.com/rjbatista/tm1638-library/

## TM16xx chip features

Type   | segments x digits    | buttons      | interface
------ | -------------------- | ------------ | -----------
TM1637 | 8 x 6 (common anode) | 8 x 2 single | DIO/CLK
TM1638 | 10 x 8               | 8 x 3 multi  | DIO/CLK/STB
TM1640 | 8 x 16               | n/a          | DO/CLK
TM1668 | 10 x 7 - 13 x 4      | 10 x 2 multi | DIO/CLK/STB

See the [documentation](/folder) for datasheets containing more information on these chips and their pinouts.

## Library structure
This library has a layered structure to simplify the support of multiple TM16xx chips.
By using a base class that provides a uniform API, your application doesn't need chip specific code.
Likewise, the library can offer common functionality in display specific classes that support multiple chips.

The figure below illustrates that concept:

![Layered structure](/images/structure.png)

## Basic usage
To use this library you need to include the class that matches the chip on your module and instantiate the object:
```C++
#include <TM1638.h>

TM1638 module(8, 9, 7);   // DIO=8, CLK=9, STB=7
```

In the setup() function you can set the intensity of the display, but that's not mandatory:
```C++
void setup() {
  module.setupDisplay(true, 2);   // on=true, intensity-2 (range 0-7)
  module.setDisplayToString("HALO");    // display simple text
}
```

In the loop() function you can use basic display methods provided by the base class:
```C++
void loop() {
  int nTime = ((millis() / 1000) / 60) * 100 + (millis() / 1000) % 60; // convert time to minutes+seconds as integer
  module.setDisplayToDecNumber(nTime, _BV(4)); // display milliseconds with dot on digit 4
}
```

The TM16xx chip makes it easy to see if a button is pressed.
To check if a button was pressed you can use the getButtons() method:
```C++
  byte btButtons=module.getButtons();
  Serial.println(btButtons, HEX);
```
Please note that while you don't need to write any code for debouncing, the button state may be reset when you display something.

## TMxxDisplay class
The _TMxxDisplay_ class adds some bytes to the memory footprint, but it provides the familiar easy to use print() and println() functions. Next to that it also provides some more advanced display methods. To use that class on top of the base class, all you need to do is instantiate it, refering to the chip specific class:
```C++
TM1638 module(8, 9, 7);   // DIO=8, CLK=9, STB=7
TM16xxDisplay display(&module, 8);    // TM16xx object, 8 digits
```

Simple print example using the TM16xxDisplay class:
```C++
#include <TM1638.h>
#include <TM16xxDisplay.h>

TM1638 module(8, 9, 7);   // DIO=8, CLK=9, STB=7
TM16xxDisplay display(&module, 8);    // TM16xx object, 8 digits

void setup() {
  display.println(F("HELLO !"));
}

int nCount=0;
void loop() {
  delay(1000);
  display.print("Count:");
  display.println(nCount++);
}
```

## TMxxMatrix class
The _TMxxMatrix_ class provides basic methods for using a LED-matrix. To use that class on top of the base class, all you need to do is instantiate it, refering to the chip specific class:
```C++
TM1640 module(9, 10);    // DO=9, CLK=10
#define MATRIX_NUMCOLUMNS 16
#define MATRIX_NUMROWS 8
TM16xxMatrix matrix(&module, MATRIX_NUMCOLUMNS, MATRIX_NUMROWS);    // TM16xx object, columns, rows
```
Note that the TM1640 has sufficient outputs to drive two 8x8 matrices.

These methods can be used to set the pixels of the matrix:
```C++
  matrix.setAll(true);    // set all pixels on
  matrix.setPixel(5,6, true);   // set one pixel on
  matrix.setPixel(3,2, false);   // set another pixel off
```

## More information
See the [library examples](/examples) for more information on how to use this library.

## New in this library
Original library functionality:
- Support for the TM1638 and TM1640;
- Support for common anode TM1638 module;
- Helper methods for displaying numbers in decimal, hexadecimal and binary;
- Support for multiple chained tm1638;
- Reading simultaneous button presses;
- Support for dimming the display and LEDs;
- Support for writing text;
- Support for module in inverted position.

Added library functionality:
- Revised library structure to simplify support of other TM16xx chips.
- Basic functionality in base class for a uniform API.
- Support for TM1637. Note: TM1637 does not support simultaneous button presses.
  (method derived from https://github.com/avishorp/TM1637 but using pins in standard output mode when writing).
- Support for TM1668. Note: TM1668 can be used in 10x7 - 13x4 display modes.
- Reduced required RAM memory by using PROGMEM fonts.
- Support for ATtiny44A and ESP8266 in addition to regular Arduinos.
- Separate classes for LED matrix and advanced LED display support.
- Simple display of text and numbers using familiar print() and println() methods.
- Added [library examples](/examples). See also [original examples](https://github.com/rjbatista/tm1638-library/examples).

## Features & limitations
- The current version of this library supports ESP8266 and Atmel ATmega328 and ATmega168 MCUs. Due to the required memory, the smallest ATtiny MCU supported is the ATtiny44. Please let me know if you've successfully used this library with other MCUs.
- Currently there is no specific support for daisychaining multiple chips and using combined displays. Please note that the TM1640 does support up to 16 digits or an 8x16 LED matrix and it is possible to use multiple display objects for multiple different modules. See the TM1638_TM1637ex_two_modules example. 
- I don't have the [QYF-TM1638 module](http://arduinolearning.com/code/qyf-tm1638-and-arduino-module.php) (TM138 with common anode display), so wasn't able to test that specific class. It may work, ...or not. Please let me know if you've tested that module.

## Disclaimer
- All code on this GitHub account, including this library is provided to you on an as-is basis without guarantees and with all liability dismissed. It may be used at your own risk. Unfortunately I have no means to provide support.
