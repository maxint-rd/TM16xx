/*!
	@file     TM1640plus.cpp
	@author   Gavin Lyons
	@brief    Arduino library Tm1640plus, Source file for TM1640 module(LED & KEY). Model 1 & Model 3.
*/

#include "TM1640plus.h"

/*!
	@brief Constructor for class TM1640plus
	
	@param clock  GPIO CLK pin
	@param data  GPIO DIO pin
	
*/
TM1640plus::TM1640plus( uint8_t clock, uint8_t data) {
	
	_DATA_IO = data;
	_CLOCK_IO = clock;

}

/*!
	@brief Display an integer and leading zeros optional
	@param number  integer to display 2^32
	@param leadingZeros  leading zeros set, true on , false off
	@param TextAlignment  left or right text alignment on display
*/
void TM1640plus::displayIntNum(unsigned long number, boolean leadingZeros, AlignTextType_e TextAlignment)
{
	char values[TM_DISPLAY_SIZE + 1];
	char TextDisplay[5] = "%";
	char TextLeft[3] = "ld";
	char TextRight[4] = "8ld";

	if (TextAlignment == TMAlignTextLeft)
		{
			strcat(TextDisplay ,TextLeft);  // %ld
		}else if ( TextAlignment == TMAlignTextRight)
		{
			strcat(TextDisplay ,TextRight); // %8ld
		}

	snprintf(values, TM_DISPLAY_SIZE + 1, leadingZeros ? "%08ld" : TextDisplay, number);
	displayText(values);
}

/*!
	@brief Display an integer in a nibble (4 digits on display)
	@param numberUpper   upper nibble integer 2^16
	@param numberLower   lower nibble integer 2^16
	@param leadingZeros  leading zeros set, true on , false off
	@param TextAlignment  left or right text alignment on display
	@note
		Divides the display into two nibbles and displays a Decimal number in each.
		takes in two numbers 0-9999 for each nibble.
*/
void TM1640plus::DisplayDecNumNibble(uint16_t  numberUpper, uint16_t numberLower, boolean leadingZeros, AlignTextType_e TextAlignment )
{
	char valuesUpper[TM_DISPLAY_SIZE + 1];
	char valuesLower[TM_DISPLAY_SIZE/2 + 1];
	char TextDisplay[5] = "%";
	char TextLeft[4] = "-4d";
	char TextRight[3] = "4d";

	 if (TextAlignment == TMAlignTextLeft)
	{
			strcat(TextDisplay ,TextLeft);  // %-4d
	}else if ( TextAlignment == TMAlignTextRight)
	{
			strcat(TextDisplay ,TextRight); // %4d
	}

	snprintf(valuesUpper, TM_DISPLAY_SIZE/2 + 1, leadingZeros ? "%04d" : TextDisplay, numberUpper);
	snprintf(valuesLower, TM_DISPLAY_SIZE/2 + 1, leadingZeros ? "%04d" : TextDisplay, numberLower);

	 strcat(valuesUpper ,valuesLower);
	 displayText(valuesUpper);
}

/*!
	@brief Display a text string  on display
	@param text    pointer to a character array
	@note 
		Dots are removed from string and dot on preceding digit switched on
		"abc.def" will be shown as "abcdef" with c decimal point turned on.
*/
void TM1640plus::displayText(const char *text) {
	char c, pos;
	pos = 0;
		while ((c = (*text++)) && pos < TM_DISPLAY_SIZE)  {
		if (*text == '.' && c != '.') {
			displayASCIIwDot(pos++, c);

			text++;
		}  else {
			displayASCII(pos++, c);
		}
		}
}

/*!
	@brief Display an ASCII character with decimal point turned on
	@param position The position on display 0-7 
	@param ascii The ASCII value from font table  to display 
*/
void TM1640plus::displayASCIIwDot(uint8_t position, uint8_t ascii) {
		// add 128 or 0x080 0b1000000 to turn on decimal point/dot in seven seg
	display7Seg(position, pgm_read_byte(pFontSevenSegptr +(ascii- TM_ASCII_OFFSET)) + TM_DOT_MASK_DEC);
}

/*!
	@brief Display an ASCII character on display
	@param position The position on display 0-7  
	@param ascii The ASCII value from font table  to display 
*/
void TM1640plus::displayASCII(uint8_t position, uint8_t ascii) {
	display7Seg(position, pgm_read_byte(pFontSevenSegptr + (ascii - TM_ASCII_OFFSET)));
}

/*!
	@brief  Send seven segment value to seven segment
	@param position The position on display 0-7  
	@param value  byte of data corresponding to segments (dp)gfedcba 
	@note 	0b01000001 in value will set g and a on.
*/
void TM1640plus::display7Seg(uint8_t position, uint8_t value) { 
	sendCommand(TM_WRITE_LOC);
	digitalWrite(_DATA_IO, LOW);
//sendData(TM_SEG_ADR + (position << 1));
sendData((0xC0 | (0x0F & position)));  
	sendData(value);
	digitalWrite(_DATA_IO, HIGH);
}

 /*!
	@brief  Send Hexadecimal value to seven segment
	@param position The position on display 0-7  
	@param hex  hexadecimal  value (DEC) 0-15  (0x00 - 0x0F)
*/
void TM1640plus::displayHex(uint8_t position, uint8_t hex)
{
	uint8_t offset = 0;
	hex = hex % 16;
	if (hex <= 9)
	{
		display7Seg(position, pgm_read_byte(pFontSevenSegptr + (hex + TM_HEX_OFFSET)));
		// 16 is offset in reduced ASCII table for number 0
	}else if ((hex >= 10) && (hex <=15))
	{
		// Calculate offset in reduced ASCII table for AbCDeF
		switch(hex)
		{
		 case 10: offset = 'A'; break;
		 case 11: offset = 'b'; break;
		 case 12: offset = 'C'; break;
		 case 13: offset = 'd'; break;
		 case 14: offset = 'E'; break;
		 case 15: offset = 'F'; break;
		}
		display7Seg(position, pgm_read_byte(pFontSevenSegptr + (offset-TM_ASCII_OFFSET)));
	}
}
