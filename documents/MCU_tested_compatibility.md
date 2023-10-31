## Contents
[WORK IN PROGRESS]
This document lists different combinations of MCUs and TM16xx chips that have been tested to work.

## tested MCU
These MCUs have been tested to work with the TM16xx library at some point in time.

|MCU-family|MCU|Remarks|
|----------|---|-------|
|ATmega|ATmega328p, ATmega168, ATmega8| tested regular Arduino's, PRO-Mini, Nano|
|Logic Green|LGT8F328P| tested SSOP16 and QFP32 variants|
|ATtiny|ATtiny44A, ATtiny84A, ATtiny85| tested various bare chips and development boards|
|Espressive|ESP8266, ESP32||
|Raspberry PI|PI PICO RP2040||
|WCH|CH32V003|tested SSOP20 variant|

Support for ATtiny's (44A, 84A, 85), ESP8266, ESP32, LGT8F328P, Pi Pico, in addition to regular Arduinos.

[TODO]

## tested TM16xx chips

Type   | Segments x digits    | Buttons      | Interface   | Notes
------ | -------------------- | ------------ | ------------|-------------
TM1616 | 7 x 4                | n/a          | DIN/CLK/STB | 
TM1620 | 8 x 6 - 10 x 4       | n/a          | DIN/CLK/STB |
TM1628 | 10 x 7 - 13 x 4      | 10 x 2 multi | DIO/CLK/STB |
TM1630 | 7 x 5 - 8 x 4        | 7 x 1 multi  | DIO/CLK/STB |
TM1637 | 8 x 6 (common anode) | 8 x 2 single | DIO/CLK     |
TM1638 | 10 x 8               | 8 x 3 multi  | DIO/CLK/STB | Anode/Inverted/QYF*
TM1640 | 8 x 16               | n/a          | DIN/CLK     | Anode*
TM1650 | 8 x 4                | 7 x 4 single | DIO/CLK     | Not real I2C SDA/SCL
TM1652 | 8 x 5 - 7 x 6        | n/a          | DIN         | Single data line
TM1668 | 10 x 7 - 13 x 4      | 10 x 2 multi | DIO/CLK/STB |

\* Alternative configurations TM1638QYF/TM1638Anode/InvertedTM1638 and TM1640Anode are also supported.

[TODO]

## Tested Compatibility

|MCU|TM16xx|
|---|------|

[TODO]
