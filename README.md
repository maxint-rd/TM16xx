# TM16xx
Arduino TM16xx library for LED & KEY and LED Matrix modules based on TM1638, TM1637, TM1640 and similar chips. Simply use print() on 7-segment end 14-segment displays and use Adafruit GFX on matrix displays.

## TM16xx LEDs and Buttons library
This Arduino library facilitates driving LED displays using TM16xx LED driver chips.
The TM16xx chip family by Titan Micro allows driving LED displays or LED matrices.
Next to built-in high-frequency LED multiplexing, they offer control of LED brightness.
Most TM16xx chips also support reading key-scan data for button presses.
Using this library you can simply use print() on a 7-segment or 14-segment displays or use Adafruit GFX on a LED matrix.
Currently this library supports more than 25 [different TM16xx chips](#tm16xx-chip-features) in various configurations; such as the TM1616, TM1618, TM1620, TM1628, TM1630, TM1637, TM1638, TM1640, TM1650 and TM1652.
For a quick look hit the play button in this [LIVE DEMO](https://wokwi.com/projects/424388671069721601) on Wokwi.

Made by Maxint R&D. See https://github.com/maxint-rd/

Initial version was based on the [TM1638 library](https://github.com/rjbatista/tm1638-library/) by Ricardo Batista. Further inspiration from the [TM1637 library](https://github.com/avishorp/TM1637) by Avishay, the [Max72xxPanel library](https://github.com/markruys/arduino-Max72xxPanel) by Mark Ruys and the [OneButton library](https://github.com/mathertel/OneButton) by Matthias Hertel. 

## Table of contents
- [TM16xx chip features](#tm16xx-chip-features)
- [Library structure](#library-structure)
- [Library installation](#library-installation)
- [Base class usage](#base-class-usage-tm1637-tm1638-tm16)
- [Generic class (TM16xxIC)](#generic-class-tm16xxic)
- [TM16xxDisplay class](#tm16xxdisplay-class)
- [TM16xxMatrix class](#tm16xxmatrix-class)
- [TM16xxMatrixGFX class](#tm16xxmatrixgfx-class)
- [TM16xxButtons class](#tm16xxbuttons-class)
- [New in this library](#new-in-this-library)
- [Features & limitations](#features--limitations)
- [More information](#more-information)

## TM16xx chip features

The [TM16xx family](https://github.com/maxint-rd/TM16xx/wiki/TM16xx-chips-features-and-support) is quite large. Several family members found their way to Arduino compatible LED display modules, for instance these:

Type   | Segments x digits    | Buttons      | Interface   | Notes
------ | -------------------- | ------------ | ------------|-------------
TM1637 | 8 x 6 (CA)           | 8 x 2 single | DIO/CLK     |
TM1638 | 10 x 8               | 8 x 3 multi  | DIO/CLK/STB | Anode/Inverted/QYF*
TM1640 | 8 x 16               | n/a          | DIN/CLK     | Anode*
TM1650 | 8 x 4                | 7 x 4 multi  | DIO/CLK     | 

The following chips are fully supported and tested to work: TM1616, TM1618, TM1620, TM1623, TM1624, TM1628, TM1630, TM1637, TM1638, TM1640, TM1650, TM1652, TM1668.
\*) Alternative configurations TM1638QYF/TM1638Anode/InvertedTM1638, TM1618Anode and TM1640Anode are also supported.

Note that there are similar chips made by other manufacturers that may be compatible with the Titan Micro chips. For instance: the HBS640 by WINRISE is compatible with the TM1640.

As of end 2024 many more chips are supported: TM1620B, TM1623C, TM1626A, TM1616B, TM1628A, TM1629A, TM1629B, TM1629C, TM1629D, TM1636, TM1639, TM1640B, TM1642, TM1643, TM1665, TM1667. Note that while their features are supported via the [generic class TM16xxIC](#generic-class-tm16xxic), I don't have them all in my collection and they have not all been tested. Please please let me know your findings if you have tested one of them.

___NEW___  - In 2025 support was added for the [TM1621 and TM1622 LCD drivers](https://github.com/maxint-rd/TM16xx/wiki/TM16xx-chips-features-and-support#tm16xx-lcd-driver-chips), including specific support for the HT1621 based 6 x 7-segment PDC-6X1 module and the TM1622 based DM8BA10 module with 10 x 16-segment alphanumeric digits. The TM1621D was tested with a 3x7-segment mini LCD display. Other variations (TM1621B, TM1621C, TM1622B) were not tested, but may work.

For a full overview of all the chips and their current level of support go to [TM16xx chips features and support](https://github.com/maxint-rd/TM16xx/wiki/TM16xx-chips-features-and-support).

See the [documents folder](/documents) for datasheets containing more information about these chips and their pinouts.

## Library structure
This library has a layered structure to simplify the support of multiple TM16xx chips.
By using a base class that provides a uniform API, your application doesn't need chip specific code.
Likewise, the library can offer common functionality in display specific classes that support multiple chips.

The figure below illustrates that concept:

![Layered structure](/images/structure.png)

## Library installation
The easiest way to install this library is using the Arduino Library Manager. Just search for "TM16xx LEDs and Buttons" and click the install button.
You can also download the [latest version](https://github.com/maxint-rd/TM16xx/archive/refs/heads/master.zip) or select one from the [releases](https://github.com/maxint-rd/TM16xx/releases) as zipfile and use Add .ZIP library in the Arduino IDE.

___NOTE: AdafruitGFX needs to be installed, even if you don't use TM16xxMatrixGFX. If you don't want to install AdafruitGFX you can remove TM16xxMatrixGFX.h and TM16xxMatrixGFX.cpp from the library directory to avoid compilation errors.___


## Base class usage (TM1637, TM1638, TM16..)
After installation you can use this library by including the class header that matches the chip on your module and then instantiate the object:
```C++
#include <TM1638.h>

TM1638 module(8, 9, 7);   // DIO=8, CLK=9, STB=7
```

In the setup() function you can initialize and set the intensity of the display, but that's not mandatory:
```C++
void setup() {
  module.begin(true, 2);   // on=true, intensity-2 (range 0-7)
  module.setDisplayToString("HALO");    // display simple text
}
```

In the loop() function you can use basic display methods provided by the [TM16xx base class ](https://github.com/maxint-rd/TM16xx/wiki/TM16xx-base-class-reference):
```C++
void loop() {
  int nTime = ((millis() / 1000) / 60) * 100 + (millis() / 1000) % 60; // convert time to minutes+seconds as integer
  module.setDisplayToDecNumber(nTime, _BV(4)); // display milliseconds with dot on digit 4
}
```

For the easy to use print() method and more advance display methods you can use the [_TM16xxDisplay_](#tm16xxdisplay-class) class.

The TM16xx chip makes it easy to see if a button is pressed.
To check if a button was pressed you can use the getButtons() method:
```C++
  uint32_t dwButtons=module.getButtons();
  Serial.println(dwButtons, HEX);
```
Please note that while you don't need to write any code for debouncing, the button state may be reset when you display something. For advanced detection of button clicks, double clicks and long presses you can use the [_TM16xxButtons_](#tm16xxbuttons-class) class.

For detailed documentation of the TM16xx base class go to the [TM16xx base class function reference](https://github.com/maxint-rd/TM16xx/wiki/TM16xx-base-class-reference).

## Generic class TM16xxIC
___NEW___  - To support a large range of TM16xx chips, the generic class TM16xxIC was added. If your chip has no chip specific header file, your chip has not been tested yet, but may still be supported via the generic class if it has supported chip features. To use it include the generic class header and specify your chip when instantiating the module object:
```C++
#include <TM16xxIC.h>

TM16xxIC module(IC_TM1638, 8, 9, 7);   // IC is TM1638, DIO=8, CLK=9, STB=7
```
These untested chips have supported features: TM1620B, TM1623C, TM1626A, TM1616B, TM1628A, TM1629, TM1629A, TM1629B, TM1629C, TM1629D, TM1636, TM1639, TM1640B, TM1642, TM1643, TM1665, TM1667.
For a full overview of all the chips and their level of support go to [TM16xx chips features and support](https://github.com/maxint-rd/TM16xx/wiki/TM16xx-chips-features-and-support)

## TM16xxDisplay class
The _TM16xxDisplay_ class adds some bytes to the memory footprint, but it provides the familiar easy to use print() and println() functions. Next to that it also provides some more advanced display methods. To use that class on top of the base class, all you need to do is instantiate it, refering to the chip specific class:
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
If you want you can combine multiple modules into a single TM16xxDisplay object. When combined print() and println() will use all available digits to print the string.
See [TM16xxDisplay class reference](https://github.com/maxint-rd/TM16xx/wiki/TM16xxDisplay-class-reference) for the provided methods.

## TM16xxMatrix class
The _TM16xxMatrix_ class provides basic methods for using a single LED-matrix module. For more advanced graphics use the [_TM16xxMatrixGFX_](#tm16xxmatrixgfx-class) class. To use the _TM16xxMatrix_ class on top of the base class, all you need to do is instantiate it, refering to the chip specific class:
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
See [TM16xxMatrix.h](/src/TM16xxMatrix.h) for the provided methods.

## TM16xxMatrixGFX class
The _TM16xxMatrixGFX_ class implements the popular [Adafruit GFX](https://learn.adafruit.com/adafruit-gfx-graphics-library/overview) interface to drive one or more TM16xx based LED-matrix modules. To use the _TM16xxMatrixGFX_ class you first need to include the proper header files:
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

In addition all the [Adafruit GFX methods](https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives) can be used, e.g.:
```C++
  matrix.drawChar(0, 0, 'A', HIGH, LOW, 1);
  matrix.drawLine(0, matrix. height(), matrix.width(), 0, HIGH);
  matrix.drawRect(0, 0, 6, 6, HIGH);
```  

Multiple identical modules can be combined to form a large matrix. The data line
can be shared to reduce the number of pins:
```C++
  TM1640 module(D7, D5);    // For ESP8266/WeMos D1-mini: DIN=D7/13/MOSI, CLK=D5/14/SCK
  TM1640 module2(D7, D6);   // For ESP8266/WeMos D1-mini: shared DIN=D7/13/MOSI, different CLK
  TM16xx * modules[]={&module,&module2};      // put modules in an array
  TM16xxMatrixGFX matrix(modules, MODULE_SIZECOLUMNS, MODULE_SIZEROWS, 2, 1);    // modules, size of each module, size combined
```  
See  [Adafruit GFX documentation](https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives) and [TM16xxMatrixGFX.h](/src/TM16xxMatrixGFX.h) for the provided methods. See the [library examples](/examples) for more information.

___NOTE: AdafruitGFX needs to be installed, even if you don't use TM16xxMatrixGFX. If you don't want to install AdafruitGFX you can remove TM16xxMatrixGFX.h and TM16xxMatrixGFX.cpp from the library directory to avoid compilation errors. To reduce this dependency, the TM16xxMatrixGFX class may be moved to a separate library in future versions___


## TM16xxButtons class
The _TM16xxButtons_ class enlarges the footprint a bit, but based on the popular [OneButton library](https://github.com/mathertel/OneButton) library, it adds more advanced methods to use buttons. Next to simply polling the state of each button, you can define callback functions that will be called when a button is released, clicked, double-clicked or long pressed. To use this class on top of the base class, all you need to do is include the proper headers and instantiate the buttons object, refering to the chip specific class, for example:
```C++
#include <TM1638.h>
#include <TM16xxButtons.h>

TM1638 module(8, 9, 7);   // DIO=8, CLK=9, STB=7
TM16xxButtons buttons(&module);    // TM16xx object
```

Then you define the functions you want to use to handle the button events:
```C++
void fnClick(byte nButton)
{ // byte nButton is the button-number (first button is number 0)
  Serial.print(F("Button "));
  Serial.print(nButton);
  Serial.println(F(" click."));
}
```

In setup() you need to attach the callback function:
```C++
void setup()
{
    .
    .
    buttons.attachClick(fnClick);
}
```
(BTW. Besides a click function, you can also attach a function to handle release, doubleclick and longpress events).

In loop() you need to call the tick() function that detects all state changes and calls the callback functions as needed:
```C++
void loop()
{
  buttons.tick();
  .
  .
  // do your other things
}
```
Some [TM16xx chips](#tm16xx-chip-features) support multiple simultaneous key presses. To implement a shift key, you can use the isPressed() function. See [TM16xxButtons.h](/src/TM16xxButtons.h) for the provided methods and the [Button clicks example](/examples/TM16xxButtons_clicks) for more information.

## New in this library

Added library functionality:
- Support for ATtiny's (44A, 84A, 85), ESP8266, ESP32, LGT8F328P, Pi Pico RP2040, CH32 RiscV, in addition to regular Arduinos.
- Revised library structure to simplify support of other TM16xx chips in addition to TM1638/TM1640.
- Basic functionality in base class for a uniform API.
- Added setDisplayFlipped() method to display numbers/text upside down (thanks @HoseanRC).
- Reduced required RAM memory by using PROGMEM fonts.
- Separate classes for LED matrix and advanced LED display support.
- Simple display of text and numbers on 7-segment and 14-segment displays using familiar print() and println() methods.
- Support for combining multiple 7-segment and 14-segment display modules into one large display.
- Support for the Adafruit GFX graphics library for advanced graphics on a LED matrix.
- Support for combining multiple matrix modules into one large Adafruit GFX matrix.
- Support for release, click, doubleclick and long press button detection using callback functions.
- Added [library examples](/examples).
- Support for TM1616 (suggested by @NickLplus)
- Support for TM1618 with 8x4 common cathode or 7x5 common anode displays (suggested by @ArnieO).
- Support for TM1620 (thanks @eddwhite)
- Support for TM1621 and TM1622 LCD drivers, including LCD modules PDC-6X1 and DM8BA10.
- Support for TM1623, TM1624 (tested derivement of TM16xxIC) - can be used in 11 x 7 - 14 x 4 display modes.
- Support for TM1628. Note: TM1628 can be used in 10x7 - 13x4 display modes.
- Support for TM1630 (thanks @tokuhira)
- Support for TM1637. Note: TM1637 does not support simultaneous button presses.
  (Method derived from [TM1637 library](https://github.com/avishorp/TM1637) but using pins in standard output mode when writing).
- Support for scanning all possible keys (K1, K2 and K3 lines) on TM1638.
- Full support for QYF-TM1638 module (8 digit common anode LED display and 4x4 keypad)
- Support for TM1638 in Anode Mode (10 digit common anode LED 8 segment display) (see [TM1638Anode.h](/src/TM1638Anode.h))
- Support for TM1640 in Anode Mode (8 digit common anode LED 16 segment display) (see [TM1640Anode.h](/src/TM1640Anode.h))
- Support for TM1650. Note: TM1650 can be used in 8x4 or 7x4 display mode and supports simultaneous presses on K1/K2.
- Support for TM1652. Note: TM1652 uses a single data line and fixed timing to determine the clock. Datasheet fully translated.
- Support for TM1668. Note: TM1668 can be used in 10x7 - 13x4 display modes. Datasheet partly translated.
- Support for many more TM16xx family members via TM16xxIC class: e.g. TM1620B, TM1623, TM1624, TM1626A, TM1616B, TM1628A, TM1629, TM1629A, TM1629B, TM1629C, TM1629D, TM1636, TM1639, TM1640B, TM1642, TM1643, TM1665, TM1667. List of all the chips and their level of support: [TM16xx chips features and support](https://github.com/maxint-rd/TM16xx/wiki/TM16xx-chips-features-and-support)

Functionality in original library by Ricardo Batista:
- Support for the TM1638 and TM1640, including common anode TM1638 module;
- Helper methods for displaying numbers in decimal, hexadecimal and binary;
- Support for multiple chained TM1638 and for TM1638 in inverted position;
- Support for dimming the display and LEDs and for writing text;
- Reading simultaneous button presses on TM1638;

## Features & limitations
- The current version of this library supports ESP8266/ESP32, Atmel ATmega (e.g. ATmega328 and ATmega168) and Atmel ATtiny MCUs. Due to the required memory, the smallest ATtiny MCU supported is the ATtiny44. Compatible MCUs such as LGT8F328P are also supported. Raspberry Pi Pico RP2040 is supported too (tested with core 2.5.2 by Earle Philhower). Support for CH32 RiscV was added in 2024. STM32 is expected to work too but needs further testing. Please let me know if you've successfully used this library with other MCUs.
- For TM1640 on Pi Pico [stability issues](https://github.com/maxint-rd/TM16xx/issues/34) were reported (requiring further analysis).
- The TM16xx chips offer no support for daisychaining multiple chips, but when separate Clk or Latch lines are used the Din line can be shared for combined displays.
- It is possible to define multiple display objects for multiple different modules (see the TM1638_TM1637ex_two_modules example). The library now supports combining multiple 7-segment or 14-segment modules into one display using the TM16xxDisplay class (example for combined TM16xxDisplay yet to be added). 
- The TM16xxMatrixGFX class does support combining multiple LED Matrix module into one large matrix. 
- the TM1640 supports up to sixteen 7-segment digits, eight 14-segment digits or an 8x16 LED matrix. The TM1640Anode class can be used with a Common Anode configuration to support 8 digits of 16 segments, such as the 5241BS 14-segment plus dot LED display (example for combined TM1640Anode yet to be added).
- The [QYF-TM1638 module](http://arduinolearning.com/code/qyf-tm1638-and-arduino-module.php) (TM138 with common anode display) is fully supported. Please note that while multiple buttons can be pressed, pressing more than two buttons can give faulty results due to the lack of short-out preventing diodes on the module.
- The popular TM1638 LED & KEY module comes in a number of varieties. My version has some odd button wiring sequence: S1=KS1, S5=KS2, S2=KS3, S6=KS4, S3=KS5, S7=KS6, S4=KS7, S8=KS8
- TM1628/TM1668 allow 13 segment mode, TM1623/TM1624 allow 14 segment mode. By combining G1/G2 and or omitting decimal point, thise chips can be used to display alphanumeric text on 14-Segment + DP LED displays.
- The TM1650 datasheet mentions SDA and SCL pins. The used protocol resembles I2C, but lacks addressing. For that reason this library doesn't use the I2C Wire library, but (slow) bitbanging using digitalWrite.
- The TM1652 allows more levels of LED dimming than supported. For uniformity only 8 levels of duty cycle are used, at maximum grid current.
- The TM1668 class has experimental support for using RGB LEDs on Grids 5-7. Some information about the wiring can be found in the example code. In future versions this functionality may be replaced by a specific class for using RGB LEDs. (TODO: The TM1680 has 8x24 outputs which sounds ideal for creating a 8x8 RGB matrix. Unfortunately these chips don't support individual LED brightness, only intensity of the whole display).
- The WeMOS D1 mini Matrix LED Shield and the TM1640 Mini LED Matrix 8x16 by Maxint R&D have R1 on the right-top. Call setMirror(true) to reverse the x-mirrorring.
- When using TM16xxButtons, the amount of memory used can become too large. To preserve RAM memory on smaller MCUs such as the ATtiny84 and ATtiny85, the number of buttons tracked is limited to 2 combined button presses. This can be changed by setting the maximum button slots in the TM16xxButtons.h header file:
```C++
#define TM16XX_BUTTONS_MAXBUTTONSLOTS 2   // Note: changing this define requires recompilation of the library
```
- Unfortunately ATtiny44/45 and smaller don't have enough flash to support both TM16xxDisplay and TM16xxButtons classes. However, it is possible to combine the module base class with only TM16xxButtons or only TM16xxDisplay.
- An experimental RAM implementation using dynamic memory allocation is available, but not suitable for small MCUs as using malloc/free will increase the required FLASH program space by over 600 bytes. Modify the TM16XX_OPT_BUTTONS_... defines in the header file at your own risk.

## More information

### Examples
See the [library examples](/examples) for more information on how to use this library. See also the [original examples](https://github.com/rjbatista/tm1638-library/tree/master/examples) by Ricardo Batista. Most will still work or only require minor changes.

### Real world devices using a TM16xx chip
Some users found a TM16xx chip in their device and shared their experience:
- [TM1628 in a Royal Sovereign BDH-450 dehumidifier](https://github.com/maxint-rd/TM16xx/issues/45)  - nice mod to make a dumb device smarter
- [TM1652 in the Xiaomi XIAO AI Smart Alarm Clock](https://github.com/maxint-rd/TM16xx/issues/41#issue-1940161388) - make it smarter with NTP
- [TM1680 in the Denver BTL - 350 bluetooth speaker](https://github.com/maxint-rd/TM16xx/issues/2#issuecomment-1406738635) - make sense of a 11x11 RGB matrix
- [TM1680 in the thermostat MH3901-Z from MCO HOME](https://github.com/maxint-rd/TM16xx/issues/2#issuecomment-1501193063) - how PCB designers make developers crazy
- [TM1616 in the Gosund SW7 dual-channel dimmer](https://github.com/maxint-rd/TM16xx/issues/22#issue-1285063037) - make an ESP8285 dimmer talk to Tasmota

If you happen to own a device featuring a TM16xx chip, feel free to open a new issue, sharing your experience. All contributions are appreciated!

### Links
- Manufacturer: [Titan Micro Electronics](http://www.titanmec.com/index.php/en/index/index.html) - [LED driver datasheets](http://www.titanmec.com/index.php/en/product/lists/typeid/88/p/1.html)
- List of all the chips and their level of support: [TM16xx chips features and support](https://github.com/maxint-rd/TM16xx/wiki/TM16xx-chips-features-and-support)
- Original TM1638/TM1640 library: [/rjbatista/tm1638-library](https://github.com/rjbatista/tm1638-library)
- TM1637 library used for reference: [/avishorp/TM1637](https://github.com/avishorp/TM1637)
- A TM1637 library optimized for speed and size: [/Erriez/ErriezTM1637](https://github.com/Erriez/ErriezTM1637)
- TM1637 library optimized for ATtiny85 with alphanumeric animations: [/jasonacox/TM1637TinyDisplay](https://github.com/jasonacox/TM1637TinyDisplay) and [online animator](https://jasonacox.github.io/TM1637TinyDisplay/examples/7-segment-animator.html)
- TM1650 library that uses the Wire interface: [/mozgy/Mozz_TM1650](https://github.com/mozgy/Mozz_TM1650)
- A TM16XX library with an interesting generic approach: [sufzoli/TM16XX](https://github.com/sufzoli/TM16XX)
- MAX7219 LED Matrix library: [/markruys/arduino-Max72xxPanel](https://github.com/markruys/arduino-Max72xxPanel)
- OneButton multi-state buttons: [/mathertel/OneButton](https://github.com/mathertel/OneButton)
- Adafruit GFX library: [/adafruit/Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library)
- Adafruit GFX documentation: [adafruit-gfx-graphics-library](https://learn.adafruit.com/adafruit-gfx-graphics-library)
- Matrix transposition used in TM1638QYF: [Flipping_Mirroring_and_Rotating](https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#Anti-Diagonal)

## Disclaimer
- All code on this GitHub account, including this library is provided to you on an as-is basis without guarantees and with all liability dismissed. It may be used at your own risk. Unfortunately I have no means to provide support.
