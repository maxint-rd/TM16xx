/*
TM16xxIC - Library for TM16xx led display drivers.
Generic driver library to support multiple variants by specfied chip characteristics.

This generic class supports most 2 and 3 wire TM16xx chips, but has a larger footprint than
the chip specific class. E.g. for TM1637 The overhead is 1144 bytes flash and 16 bytes RAM
(tested on CH32V003, optimized for size -Os without LTO)

Other than having a different constructor the API of the generic class is compatible with 
the chip-specific classes.

Part of the TM16xx library by Maxint. See https://github.com/maxint-rd/TM16xx
The Arduino TM16xx library supports LED & KEY and LED Matrix modules with chips like TM1638 and TM1640, as well as individual chips.
Simply use print() on 7-segment or 14-segment displays and use Adafruit GFX on matrix displays.

Made by Maxint R&D. See https://github.com/maxint-rd
*/

#include "TM16xxIC.h"

/*
** Private methods
*/
uint8_t TM16xxIC::countMaxSEG(void)
{ // count the maximum number of SEG pins that can be used, based on the bits set in _ctrl.seg_mask
byte nCnt=0;
  for(byte n=0; n<16; n++)
    if(_ctrl.seg_mask&bit(n)) nCnt++;
  return(nCnt);
}

/*
** Public methods
*/

TM16xxIC::TM16xxIC(if_ctrl_tm16xx ctrl, byte dataP, byte clockP, byte strobeP, byte numGrdUsed)
	: TM16xx(dataP, clockP, strobeP, -1, -1)
{
  // Many TM16xx chips have 4 display modes (minSeg x 7 Grid - maxSeg x 4 Grid), some less or none (i.e. only one mode)
  // numDigits is used to determine the number of GRD.
  // The maximum number of segments is count_bits(ctrl.seg_mask).
  // The upper segment pins are shared with the upper grid pins, so select appropriate mode based on numDigits
  _ctrl=ctrl;
  if(_ctrl.if_type==TM16XX_IFTYPE_2WACK || _ctrl.if_type==TM16XX_IFTYPE_2WSER)
  {
    this->strobePin=dataP;
    digitalWrite(this->strobePin, HIGH);
  }

  // Many TM16xx chips support multiplexing of up to 3 SEG pins to allow using them as GRD instead.
  // The high nibble of _ctrl.seg_bytes specifies how many SEG pins can be used as GRD.
  // Depending on numDigits the display mode is set by setupDisplay(), based on these settings.
  // TODO: TM1680/TM1681 have deviating memory layout: addresses are for 96 half-byte memory locations
  // TODO: TM1650 has 8x4/7x4 display mode, having 1 pin multiplexed as either SEG pin or Key interrupt pin.
  if(numGrdUsed==0 || numGrdUsed>_ctrl.grid_len)  // we can't use more GRD pins than the number of memory positions
    numGrdUsed=_ctrl.grid_len;
  _maxDisplays=numGrdUsed;   // reset value set by TM16xx constructor parameter of -1
  this->digits=_maxDisplays; // reset value set by TM16xx constructor parameter of -1

  // calculate total SEG/GRD pins possible. NIBBLE_HIGH(_ctrl.seg_bytes) specifies how many SEG lines can be used as GRD
  _maxSegments=countMaxSEG();   // _ctrl.seg_mask specifies which memory bits can be used to set SEG on/off
  if(NIBBLE_HIGH(_ctrl.seg_bytes)!=TM16XX_IC_SEGMUX_0)
  {
    byte numSegGrdPins=_maxSegments+_ctrl.grid_len-(NIBBLE_HIGH(_ctrl.seg_bytes)>>4);
    _maxSegments=numSegGrdPins-_maxDisplays;        // adjust for SEG pins used as GRD
  }

  // NOTE: CONSTRUCTORS SHOULD NOT CALL DELAY() <= gives hanging on certain ESP8266/ESP32 cores as well as on LGT8F328P 
  // Using micros() or millis() in constructor also gave issues on LST8F328P and CH32V003.
  // Some TM16xx chips uses bit-timing to communicate, so clearDisplay() and setupDisplay() cannot be called in constructor.
  // Call begin() in setup() to clear the display and set initial activation and intensity.
  // To make begin() optional, sendData() implicitely calls begin(). Although it will execute only once, it may cause a
  // second call to clearDisplay() by an explicit call to clearDisplay(). Therefor sketches should preferably call begin().
}

// constructor for chips with only data and clock
TM16xxIC::TM16xxIC(if_ctrl_tm16xx ctrl, byte dataP, byte clockP) : TM16xxIC(ctrl, dataP, clockP, dataP) {}


void TM16xxIC::bitDelay()
{
	delayMicroseconds(_ctrl.bit_delay);
	// NOTE: on TM1637 reading keys should be slower than 250Khz (see datasheet p3)
	// for that reason the delay between reading bits should be more than 4us
	// When using a fast clock (such as ESP8266) a delay is needed to read bits correctly
}

void TM16xxIC::start()
{	
  if(_ctrl.if_type==TM16XX_IFTYPE_2WSER)
  {	// TM1640: The starting condition of data input is: when CLK is high, the DIN becomes low from high;
    digitalWrite(dataPin, LOW);
    digitalWrite(clockPin, LOW);
    bitDelay();
  }
  else
    TM16xx::start();
}

void TM16xxIC::send(byte data)
{
  TM16xx::send(data);

	// MOLE 180514: chips like TM1637 uses acknowledgement after sending the data
	// (method derived from https://github.com/avishorp/TM1637 but using pins in standard output mode when writing)
  if(_ctrl.if_type==TM16XX_IFTYPE_2WACK)
  {
    // unlike TM1638/TM1668 and TM1640, chips like TM1637 uses an ACK to confirm reception of command/data
    // read the acknowledgement
    // TODO? return the ack?
    digitalWrite(clockPin, LOW);
    pinMode(dataPin, INPUT);
    bitDelay();
    digitalWrite(clockPin, HIGH);
    bitDelay();
    uint8_t ack = digitalRead(dataPin);
    if (ack == 0)
      digitalWrite(dataPin, LOW);
    pinMode(dataPin, OUTPUT);
  }
  else if(_ctrl.if_type==TM16XX_IFTYPE_2WSER)
  { 	// MOLE 180514: TM1640 wants data and clock to be low after sending the data
    digitalWrite(clockPin, LOW);		// first clock low
    digitalWrite(dataPin, LOW);			// then data low
    bitDelay();
  }
}

void TM16xxIC::stop()
{ // to stop chips like TM1637 expects the clock to go high, when strobing DIO high
  if(_ctrl.if_type==TM16XX_IFTYPE_2WACK)
  {
    digitalWrite(strobePin, LOW);
    digitalWrite(clockPin, LOW);
    bitDelay();
    digitalWrite(clockPin, HIGH);
    digitalWrite(strobePin, HIGH);
    bitDelay();
  }
  else if(_ctrl.if_type==TM16XX_IFTYPE_2WSER)
  { // TM1640: The ending condition is: when CLK is high, the DIN becomes high from low.
    //bitDelay();     // extra delay for RP2040, matrix test showed weird behavior on some TM1640 during stop()
    digitalWrite(clockPin, HIGH);
    //bitDelay();     // extra delay for RP2040, matrix test showed weird behavior on some TM1640 during stop()
    digitalWrite(dataPin, HIGH);
    bitDelay();
  }
  else
    TM16xx::stop();
}


void TM16xxIC::setupDisplay(bool active, byte intensity)
{
  // Set display mode. Many TM16xx chips support modes with 4 to 7 GRD-lines.
  // Upper segment pins are shared with the upper grid pins, so select appropriate mode based on _maxDisplays
  byte seg_mux=NIBBLE_HIGH(_ctrl.seg_bytes);
  if(seg_mux!=TM16XX_IC_SEGMUX_0)
    sendCommand(_maxDisplays==7 ? TM16XX_CMD_MODE_7GRID : (_maxDisplays==6 ? TM16XX_CMD_MODE_6GRID : (_maxDisplays==5 ? TM16XX_CMD_MODE_5GRID : TM16XX_CMD_MODE_4GRID)));

  // Switch display on/off and set intensity 
  sendCommand(TM16XX_CMD_DISPLAY | (active ? 8 : 0) | min(7, intensity));
}


void TM16xxIC::setAlphaNumeric(bool fAlpha, const byte *pMap)    // const byte aMap[]
{
  fAlphaNumeric=fAlpha;
  pSegmentMap=pMap;
}

uint16_t TM16xxIC::flipSegments16(uint16_t uSegments)
{
  if(this->flipped)
  { // Flip the character 180 degrees by some clever bit-manipulation.
    // For 7 segments: swap segment A, B and C with D, E and F; leave G and DP intact
    // (see PR #58 and issue #20 for info and references)
    //   byte xored = (data ^ (data >> 3)) & (7);
    //   data = data ^ (xored | (xored << 3));
    //   pos = this->digits - 1 - pos;
    
    // For 14-segments: a,b,c should be swapped with d,e,f, then g1 should be swapped with g2, 
    //   then h,j,k should be swapped with l,m,n and the dot be left behind.
    // You can split the variable into 3 parts:
    //    & 0x0E07 (0000 1110 0000 0111: abc and hjk)
    //    & 0x7038 (0111 0000 0011 1000: def and lmn)
    //    & 0x01C0 (0000 0001 1100 0000: dot, g1 and g2)
    // Shift the first part to left by 3 bits, Shift the second part to right by 3 bits, swap g1 and g2 in the third part, then OR all the parts together.
    // (Bit swapping: https://graphics.stanford.edu/~seander/bithacks.html#SwappingBitsXOR     @HoseanRC - Thank you!)
    //
    uint16_t part1=uSegments & 0x0E07; // (abc and hjk)
    uint16_t part2=uSegments & 0x7038; // (def and lmn)
    uint16_t part3=uSegments & 0x01C0; // (dot, g1 and g2)
    uint16_t  xored= ((part3 >> 6) ^ (part3 >> 8)) & 1;
    part3 = part3 ^ ((xored << 6) | (xored << 8));
    uSegments = part1<<3 | part2>>3 | part3;
  }
  return(uSegments);
}

void TM16xxIC::sendAsciiChar(byte pos, char c, bool fDot, const byte font[])
{ // Method to send an Ascii character to the display.
  // This method is also called by TM16xxDisplay.print() to display characters.
  // The base class uses the default 7-segment font to find the LED pattern.
  // Derived classes for multi-segment displays or alternate layout displays can override this method.
  if(!fAlphaNumeric)
    TM16xx::sendAsciiChar(pos, c, fDot, font);    // the base method calls sendChar, which will do 7-segment flipping if needed
  else
  {
    uint16_t uSegments= pgm_read_word(TM16XX_FONT_15SEG+(c - 32));
    if(this->flipped)
    {
      uSegments=flipSegments16(uSegments);
      pos = this->digits - 1 - pos;
    }
    setSegments16(uSegments | (fDot ? 0b10000000 : 0), pos);
  }
}

uint16_t TM16xxIC::mapSegments16(uint16_t segments, const byte *pMap)
{ // Map the segments to another location if that's requested.
  // The segment map is an array of _maxSegments long of which each element specfies the remapped position.
  // Usually segment A is mapped to pin GRID1 of the TM16xx. Using segment mapping this can become any other pin.
  if(pMap)
  {
    uint16_t nSegmentsMapped=0;
    for(byte n=0; n<16; n++)
      nSegmentsMapped|=((segments&bit(n))?bit(pMap[n]):0);
    segments=nSegmentsMapped;
  }
  return(segments);
}

void TM16xxIC::setSegments(byte segments, byte position)
{	// Set 8 leds on common grd as specified.
  setSegments16(segments, position);
}

void TM16xxIC::setSegments16(uint16_t segments, byte position)
{	// Set up to 16 leds on common grd as specified.
  // The display memory orginization is defined by _ctrl.grid_len, _ctrl.seg_bytes and _ctrl.seg_mask

  // Many TM16xx chips support more than 8 segments per digit. Often these segments
  // are not stored as continuous bits; some bits are skipped.
  // The segment mask in _ctrl.seg_mask defines which bits need to be used.

  // TM1618 for instance supports up to 8 segments, spread out in two bytes to SEG1-SEG5 and SEG12-SEG14. The segment mask is 0x381F (00111000 00011111)
  // TM1668 supports up to 13 segments, spread out in two bytes to SEG1-SEG10 and SEG12-SEG14. The segment mask is 0x3BFF (00111011 11111111)
  byte seg_bytes=NIBBLE_LOW(_ctrl.seg_bytes);

  // Map segments if specified for alternative segment wiring.
  if(pSegmentMap)
    segments=mapSegments16(segments, pSegmentMap);
    
  if(position<_maxDisplays)
  {
    if(seg_bytes==TM16XX_IC_SEGBYTES_1 || seg_bytes==TM16XX_IC_SEGBYTES_1SK)
    {
      sendData(seg_bytes==TM16XX_IC_SEGBYTES_1SK ? position<<1 : position, (byte)segments);
    }
    else if(seg_bytes==TM16XX_IC_SEGBYTES_2)
    { // send up to 16 segments, spread the bits according the mask in _ctrl.seg_mask 
      uint16_t seg16=0;
      byte maskLow=_ctrl.seg_mask&0x00FF;
      byte maskHigh=_ctrl.seg_mask>>8;
      for(byte nBit=0; nBit<16; nBit++)
      {
        if(_ctrl.seg_mask & bit(nBit))
        {
          if(segments & 0x01) seg16|=bit(nBit);
          segments>>=1;
        }
      }
      sendData(position << 1, ((byte)seg16)&maskLow);
      sendData((position << 1) | 1, ((byte)(seg16>>8))&maskHigh);
    }
  }
}


void TM16xxIC::clearDisplay()
{ // NOTE: TM16xx class assumes chips only have 2 bytes per digit when it uses >8 segments
  byte seg_bytes=NIBBLE_LOW(_ctrl.seg_bytes);
  for(byte nPos=0; nPos<_ctrl.grid_len; nPos++)
  { // set all segments OFF for each memory position
    if(seg_bytes==TM16XX_IC_SEGBYTES_1 || seg_bytes==TM16XX_IC_SEGBYTES_1SK)
    { // only send one byte per position
      sendData(seg_bytes==TM16XX_IC_SEGBYTES_1SK ? nPos<<1 : nPos, 0);
    }
    else if(seg_bytes==TM16XX_IC_SEGBYTES_2)
    { // send two bytes
      sendData(nPos << 1, 0);
      sendData((nPos << 1) | 1, 0);
    }
  }
}

uint32_t TM16xxIC::getButtons(void)
{	// Keyscan data is received in different numbers of bytes and extracted in different methods.
  // The getButtons() method tries to return a 32-bit value containing 1 bit per button.
  // The maximum number of keys for all known TM16xx chips is 32 (for TM1629/TM1629D).
  //
  // ctrl.key_scan specifies the method and the number of bytes to receive
  // ctrl.key_mask specifies the bits that store key state
  // The maximum size of the keyscan memory for all known TM16xx chips is 5 (e.g. for TM1668).
  // The method of decoding keys differs per chip. In principal we try to push all bits read into a 32-bit value.
  
  // Example configurations:
  //    TM1618: 5x1 keys, 3 bytes memory, key mask: 00010010 (0x12), per byte twice K2 in bit 1 and 4, bit is 1 when key is pressed.
  //    TM1638: 8x3 keys, 4 bytes memory, key mask: 01110111 (0x77), per byte twice K3/K2/K1), bit is 1 when key is pressed.
  //    TM1668: 10x2 keys, 5 bytes memory, key mask: 00011011 (0x1B), per byte twice K1/K2), bit is 1 when key is pressed.
  //    TM1637: 8x2 keys single, 1 byte memory. All 1 when nothing pressed bit 3 low for K1, bit 4 low for K2. Inverted key number in bits 0-2 for keys 0-7
  uint32_t keys32 = 0;
  byte received;
  byte key_method=NIBBLE_HIGH(_ctrl.key_scan);
  byte key_length=NIBBLE_LOW(_ctrl.key_scan);

  if(!key_length)
    return(0);

  // Send read command and read the keys key_length times
  start();
  send(TM16XX_CMD_DATA_READ);		// send read buttons command
  bitDelay();  // the most TM16xx chips require a Twait of minimal. 1us.

  for (int i = 0; i < key_length; i++)
  {
    received=receive();

    if(key_method==TM16XX_KD_PRESS_HI)
    { // each key pressed has a corresponding high bit. Fill the return value according the key mask
      for(byte nBit=0; nBit<8; nBit++)
      {
        if(_ctrl.key_mask & bit(nBit))
        {
          if(received & 0x01) keys32|=bit(31);
          keys32>>=1;
        }
        received>>=1;
      }
    }
    else if(key_method==TM16XX_KD_PRESS_NI)
    {   // eg. TM1637: inverted key number in bits 0-2 for keys 0-7, bit 3 low for K1, bit 4 low for K2
      byte nKey=((~received) & 0x07);
      if((~received)&bit(4))
        keys32=bit(nKey);        // K1
      else if((~received)&bit(3))
        keys32=(bit(nKey))<<8;   //K2
    }
    // TODO: support method TM16XX_KD_PRESS_LO (TM1636) and TM16XX_KD_PRESS_NN (TM1650)
  }
  stop();
  return(keys32);
}