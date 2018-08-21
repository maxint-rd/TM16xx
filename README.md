# TM16xx
Arduino TM16xx library for LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 and similar chips. Simply use print() on 7-segment and use Adafruit GFX on matrix.

## TM16xx LEDs and Buttons library
This Arduino library facilitates driving LED displays using TM16xx LED driver chips.
The TM16xx chip family allows driving 7-segment LED displays or LED matrices.
Next to built-in high-frequency LED multiplexing, they offer control of LED brightness.
Most TM16xx chips also support reading key-scan data for button presses.
Currently this library supports the TM1637, TM1638, TM1640 and TM1668 chips.
Simply use print() on a 7-segment display and use Adafruit GFX on a LED matrix.
The library structure is designed to add support for other TM16xx chips without much effort.

Made by Maxint R&D. See https://github.com/maxint-rd/

Based on the [TM1638 library](https://github.com/rjbatista/tm1638-library/) by Ricardo Batista. Further inspiration from the [TM1637 library](https://github.com/avishorp/TM1637) by Avishay and  the [Max72xxPanel library](https://github.com/markruys/arduino-Max72xxPanel) by Mark Ruys. 

## TM16xx chip features

Type   | segments x digits    | buttons      | interface
------ | -------------------- | ------------ | -----------
TM1637 | 8 x 6 (common anode) | 8 x 2 single | DIO/CLK
TM1638 | 10 x 8               | 8 x 3 multi  | DIO/CLK/STB
TM1640 | 8 x 16               | n/a          | DIN/CLK
TM1668 | 10 x 7 - 13 x 4      | 10 x 2 multi | DIO/CLK/STB

See the [documents folder](/documents) for datasheets containing more information on these chips and their pinouts.

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
The _TMxxMatrix_ class provides basic methods for using a single LED-matrix module. For more advanced graphics use the _TMxxMatrixGFX_ class. To use the _TMxxMatrix_ class on top of the base class, all you need to do is instantiate it, refering to the chip specific class:
```C++
TM1640 module(9, 10);    // DIN=9, CLK=10
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

## TMxxMatrixGFX class
The _TMxxMatrixGFX_ class implements the popular [Adafruit GFX](https://learn.adafruit.com/adafruit-gfx-graphics-library/overview) interface to drive one or more TM16xx based LED-matrix modules. To use the _TMxxMatrixGFX_ class you first need to include the proper header files:
```C++
#include <Adafruit_GFX.h>
#include <TM1640.h>
#include <TM16xxMatrixGFX.h>
```

Then you can instantiate the TM16xxMatrixGFX class, refering to the chip specific class:
```C++
TM1640 module(13, 14);    // For ESP8266/WeMos D1-mini: DIN=D7/13/MOSI, CLK=D5/14/SCK
#define MATRIX_NUMCOLUMNS 8
#define MATRIX_NUMROWS 8
TM16xxMatrixGFX matrix(&module, MATRIX_NUMCOLUMNS, MATRIX_NUMROWS);    // TM16xx object, columns, rows
```
Note that the TM1640 has sufficient outputs to drive two 8x8 matrices. The WeMOS D1 Mini Matrix LED Shield also uses the TM1640, but has only one 8x8 matrix.

These methods can be used to draw on the matrix:
```C++
  matrix.setIntensity(1);         // Use a value between 0 and 7 for brightness
  matrix.fillScreen(LOW);         // Clear the matrix
  matrix.drawPixel(1, 4, HIGH);   // set one pixel in the memory bitmap on
  matrix.write();                 // Send the memory bitmap to the display
```

In addition all the Adafruit GFX methods can be used, e.g.:
```C++
  matrix.drawChar(0, 0, 'A', HIGH, LOW, 1);
  matrix.drawLine(0, matrix. height(), matrix.width(), 0, HIGH);
```  

Multiple identical modules can be combined to form a large matrix. The data line
can be shared to reduce the number of pins:
```C++
  TM1640 module(D7, D5);    // For ESP8266/WeMos D1-mini: DIN=D7/13/MOSI, CLK=D5/14/SCK
  TM1640 module2(D7, D6);   // For ESP8266/WeMos D1-mini: shared DIN=D7/13/MOSI, different CLK
  TM16xx * modules[]={&module,&module2};      // put modules in an array
  TM16xxMatrixGFX matrix(modules, MODULE_SIZECOLUMNS, MODULE_SIZEROWS, 2, 1);    // modules, size of each module, size combined
```  
See [library examples](/examples) for more information.


## More information

### Examples
See the [library examples](/examples) for more information on how to use this library. See also the [original examples](https://github.com/rjbatista/tm1638-library/examples) by Ricardo Batista. Most will still work or only require minor changes.

### Links
- Manufacturer: [Titan Micro Electronics](http://www.titanmec.com/index.php/en/product/lists/typeid/59/p/1.html)
- Original TM1638/TM1640 library: https://github.com/rjbatista/tm1638-library
- TM1637 library used for reference: https://github.com/avishorp/TM1637
- A TM1637 library optimized for speed and size: https://github.com/Erriez/ErriezTM1637
- MAX7219 LED Matrix library: https://github.com/markruys/arduino-Max72xxPanel

## New in this library
Original library functionality:
- Support for the TM1638 and TM1640, including common anode TM1638 module;
- Helper methods for displaying numbers in decimal, hexadecimal and binary;
- Support for multiple chained TM1638 and for TM1638 in inverted position;
- Support for dimming the display and LEDs;
- Support for writing text;
- Reading simultaneous button presses on TM1638;

Added library functionality:
- Revised library structure to simplify support of other TM16xx chips.
- Basic functionality in base class for a uniform API.
- Support for TM1637. Note: TM1637 does not support simultaneous button presses.
  (Method derived from [TM1637 library](https://github.com/avishorp/TM1637) but using pins in standard output mode when writing).
- Support for TM1668. Note: TM1668 can be used in 10x7 - 13x4 display modes.
- Reduced required RAM memory by using PROGMEM fonts.
- Support for ATtiny44A and ESP8266 in addition to regular Arduinos.
- Separate classes for LED matrix and advanced LED display support.
- Simple display of text and numbers on7-segment displays using familiar print() and println() methods.
- Support for the Adafruit GFX graphics library for advanced graphics on a LED matrix.
- Support for combining multiple modules into one large Adafruit GFX matrix.
- Added [library examples](/examples).

## Features & limitations
- The current version of this library supports ESP8266 and Atmel ATmega328 and ATmega168 MCUs. Due to the required memory, the smallest ATtiny MCU supported is the ATtiny44. Please let me know if you've successfully used this library with other MCUs.
- Currently there is no specific support for daisychaining multiple chips and using combined displays. Please note that the TM1640 does support up to 16 digits or an 8x16 LED matrix and it is possible to use multiple display objects for multiple different modules. See the TM1638_TM1637ex_two_modules example. 
- I don't have the [QYF-TM1638 module](http://arduinolearning.com/code/qyf-tm1638-and-arduino-module.php) (TM138 with common anode display), so wasn't able to test that specific class. It may work, ...or not. Please let me know if you've tested that module.
- The TM1668 class has experimental support for using RGB LEDs on Grids 5-7. Some information about the wiring can be found in the example code. Most likely future versions will have a specific class for using RGB LEDs. The TM1680 has 8x24 outputs which sounds ideal for creating a 8x8 RGB matrix. Unfortunately these chips don't support individual LED brightness, only intensity of the whole display.
- The WeMOS D1 mini Matrix LED Shield has R1 on SEG8 instead of SEG1. Call setMirror(true) to reverse the x-mirrorring.

## Disclaimer
- All code on this GitHub account, including this library is provided to you on an as-is basis without guarantees and with all liability dismissed. It may be used at your own risk. Unfortunately I have no means to provide support.
