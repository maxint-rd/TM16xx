# TM16xx
TM16xx library for Arduino. Supports LED and KEY modules based on TM1638 and similar chips.

TM16xx LEDs and Buttons library
-------------------------------
A library for interacting an Arduino with a TM1637/TM1638/TM1640/TM1668.
The TMxx chip family allows driving 7-segment LED displays or LED matrices.
Most TMxx chips also support reading key-scan data.
Next to built-in high-frequency LED multiplexing, they offer control of LED brightness.

Made by Maxint R&D. See https://github.com/maxint-rd/
Based on TM1638 library by Ricardo Batista, see https://github.com/rjbatista/tm1638-library/

## TM16xx chip features
-----------------------

Type   | segments x digits    | buttons      | interface
------ | -------------------- | ------------ | -----------
TM1637 | 8 x 6 (common anode) | 8 x 2 single | DIO/CLK
TM1638 | 10 x 8               | 8 x 3 multi  | DIO/CLK/STB
TM1640 | 8 x 16               | n/a          | DIN/CLK
TM1668 | 10 x 7 - 13 x 4      | 10 x 2 multi | DIO/CLK/STB

## Library structure
This library has a layered structure to simplify the support of multiple TM16xx chips.
By using a base class that provides a uniform API, your application doesn't need chip specific code.
Likewise, the library can offer common functionality in display specific classes that support multiple chips.

The figure below illustrates that concept:
![Layered structure](/images/structure.png)

## Usage
See the [library examples](/examples) for information on how to use this library.

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
