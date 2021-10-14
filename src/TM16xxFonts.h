/*
TM16xxFonts.h - Font definition for TM16xx.

Copyright (C) 2011 Ricardo Batista (rjbatista <at> gmail <dot> com)
Modified by Maxint R&D. See https://github.com/maxint-rd/
 - Improvements for 7-segment alpha-numeric font
 - Additional 15-segment font

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Segment labels
 -- A --
|       |
F       B
 -- G --
E       C
|       |
 -- D --  .DP

The bits are displayed by the mapping below:
 -- 0 --
|       |
5       1
 -- 6 --
4       2
|       |
 -- 3 --  .7

*/

#ifndef TM16XXFonts_h
#define TM16XXFonts_h

// definition for standard hexadecimal numbers
const PROGMEM byte TM16XX_NUMBER_FONT[] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111, // 9
  0b01110111, // A
  0b01111100, // B
  0b00111001, // C
  0b01011110, // D
  0b01111001, // E
  0b01110001  // F
};

const PROGMEM byte MINUS = 0b01000000;

// definition for error text
const PROGMEM byte TM16XX_ERROR_DATA[] = {
  0b01111001, // E
  0b01010000, // r
  0b01010000, // r
  0b01011100, // o
  0b01010000, // r
  0,
  0,
  0
};

// Definition for the displayable ASCII chars
// (note that non-alpha-numeric characters may display as a space or a simple dash)
const PROGMEM byte TM16XX_FONT_DEFAULT[] = {
  0b00000000, // (32)  <space>
  0b10000110, // (33)	!
  0b00100010, // (34)	"
  0b01111110, // (35)	#
  0b01101101, // (36)	$
  0b00000000, // (37)	%
  0b00000000, // (38)	&
  0b00000010, // (39)	'
  0b00110000, // (40)	(
  0b00000110, // (41)	)
  0b01100011, // (42)	*
  0b00000000, // (43)	+
  0b00000100, // (44)	,
  0b01000000, // (45)	-
  0b10000000, // (46)	.
  0b01010010, // (47)	/
  0b00111111, // (48)	0
  0b00000110, // (49)	1
  0b01011011, // (50)	2
  0b01001111, // (51)	3
  0b01100110, // (52)	4
  0b01101101, // (53)	5
  0b01111101, // (54)	6
  0b00100111, // (55)	7
  0b01111111, // (56)	8
  0b01101111, // (57)	9
  0b00000000, // (58)	:
  0b00000000, // (59)	;
  0b00000000, // (60)	<
  0b01001000, // (61)	=
  0b00000000, // (62)	>
  0b01010011, // (63)	?
  0b01011111, // (64)	@
  0b01110111, // (65)	A
  0b01111111, // (66)	B
  0b00111001, // (67)	C
  0b00111111, // (68)	D
  0b01111001, // (69)	E
  0b01110001, // (70)	F
  0b00111101, // (71)	G
  0b01110110, // (72)	H
  0b00000110, // (73)	I
  0b00011110, // (74)	J
  0b01101001, // (75)	K
  0b00111000, // (76)	L
  0b00010101, // (77)	M
  0b00110111, // (78)	N
  0b00111111, // (79)	O
  0b01110011, // (80)	P
  0b01100111, // (81)	Q
  0b00110001, // (82)	R
  0b01101101, // (83)	S
  0b01111000, // (84)	T
  0b00111110, // (85)	U
  0b00101010, // (86)	V
  0b00011101, // (87)	W
  0b01110110, // (88)	X
  0b01101110, // (89)	Y
  0b01011011, // (90)	Z
  0b00111001, // (91)	[
  0b01100100, // (92)	\ (this can't be the last char on a line, even in comment or it'll concat)
  0b00001111, // (93)	]
  0b00000000, // (94)	^
  0b00001000, // (95)	_
  0b00100000, // (96)	`
  0b01011111, // (97)	a
  0b01111100, // (98)	b
  0b01011000, // (99)	c
  0b01011110, // (100)	d
  0b01111011, // (101)	e
  0b00110001, // (102)	f
  0b01101111, // (103)	g
  0b01110100, // (104)	h
  0b00000100, // (105)	i
  0b00001110, // (106)	j
  0b01110101, // (107)	k
  0b00110000, // (108)	l
  0b01010101, // (109)	m
  0b01010100, // (110)	n
  0b01011100, // (111)	o
  0b01110011, // (112)	p
  0b01100111, // (113)	q
  0b01010000, // (114)	r
  0b01101101, // (115)	s
  0b01111000, // (116)	t
  0b00011100, // (117)	u
  0b00101010, // (118)	v
  0b00011101, // (119)	w
  0b01110110, // (120)	x
  0b01101110, // (121)	y
  0b01000111, // (122)	z
  0b01000110, // (123)	{
  0b00000110, // (124)	|
  0b01110000, // (125)	}
  0b00000001, // (126)	~
};

/*
Definition for the displayable ASCII chars on a 15-segment display.

Segment labels and bit-assignment:
   |---A---|          Bit Segment     Bit Segment
   |\  |  /|          [0]       A    [ 8]      G2
   F H J K B          [1]       B    [ 9]       H
   |  \|/  |          [2]       C    [10]       J
   |-G1-G2-|          [3]       D    [11]       K
   |  /|\  |          [4]       E    [12]       L
   E N M L C          [5]       F    [13]       M
   |/  |  \|          [6]      G1    [14]       N
   |---D---| DP       [7]      DP

14 13 12 11 10 9  8  7  6 5 4 3 2 1 0
 N  M  L  K  J H G2 DP G1 F E D C B A

Note: For compatibility bits 0-7 are in the same order as A-G on a 7-segment display.
      Most characters in the font below are based on their 7-seg counterparts to keep styling alike.
*/
const PROGMEM uint16_t TM16XX_FONT_15SEG[] = {
  0b00000000, // (32)  <space>
  0b10010010000000, // (33)	!
  0b00100010, // (34)	"
  0b10010101110001, // (35)	#
  0b10010101101101, // (36)	$
  0b100100000100100, // (37)	%
  0b1101001011001, // (38)	&
  0b00000010, // (39)	'
  0b00110000, // (40)	(
  0b00000110, // (41)	)
  0b111111101000000, // (42)	*
  0b010010101000000, // (43)	+
  0b100000000000000, // (44)	,
  0b01000000, // (45)	-
  0b10000000, // (46)	.
  0b0100100000000000, // (47)	/
  0b00111111, // (48)	0
  0b00000110, // (49)	1
  0b101011011, // (50)	2
  0b101001111, // (51)	3
  0b101100110, // (52)	4
  0b101101101, // (53)	5
  0b101111101, // (54)	6
  0b00100111, // (55)	7
  0b101111111, // (56)	8
  0b101101111, // (57)	9
  0b10000000, // (58)	:
  0b100000000000010, // (59)	;
  0b1100000000000, // (60)	<
  0b101001000, // (61)	=
  0b100001000000000, // (62)	>
  0b10000110000011, // (63)	?
  0b100111011, // (64)	@
  0b101110111, // (65)	A
  0b10010100001111, // (66)	B
  0b00111001, // (67)	C
  0b10010000001111, // (68)	D
  0b101111001, // (69)	E
  0b101110001, // (70)	F
  0b100111101, // (71)	G
  0b101110110, // (72)	H
  0b10010000001001, // (73)	I
  0b00011110, // (74)	J
  0b01100001110000, // (75)	K
  0b00111000, // (76)	L
  0b101000110110, // (77)	M
  0b1001000110110, // (78)	N
  0b00111111, // (79)	O
  0b101110011, // (80)	P
  0b1000000111111, // (81)	Q
  0b1000101110011, // (82)	R
  0b101101101, // (83)	S
  0b010010000000001, // (84)	T
  0b00111110, // (85)	U
  0b1001000000110, // (86)	V
  0b101000000110110, // (87)	W
  0b101101000000000, // (88)	X
  0b00010101000000000, // (89)	Y
  0b100100000001001, // (90)	Z
  0b00111001, // (91)	[
  0b1001000000000, // (92)	\ (this can't be the last char on a line, even in comment or it'll concat)
  0b00001111, // (93)	]
  0b101000000000000, // (94)	^
  0b00001000, // (95)	_
  0b1000000000, // (96)	`
  0b101011111, // (97)	a
  0b101111100, // (98)	b
  0b101011000, // (99)	c
  0b101011110, // (100)	d
  0b101111011, // (101)	e
  0b01110001, // (102)	f
  0b101101111, // (103)	g
  0b101110100, // (104)	h
  0b00000100, // (105)	i
  0b00001110, // (106)	j
  0b100001110100, // (107)	k
  0b00110000, // (108)	l
  0b101000110110, // (109)	m
  0b10000100000100, // (110)	n
  0b101011100, // (111)	o
  0b101110011, // (112)	p
  0b101100111, // (113)	q
  0b101010000, // (114)	r
  0b101101101, // (115)	s
  0b01111000, // (116)	t
  0b00011100, // (117)	u
  0b100000000010000, // (118)	v
  0b101000000010100, // (119)	w
  0b101101000000000, // (120)	x
  0b100101000000000, // (121)	y
  0b100100000001001, // (122)	z
  0b100001001001001, // (123)	{
  0b010010000000000, // (124)	|
  0b1100100001001, // (125)	}
  0b00000001, // (126)	~
};


#endif
