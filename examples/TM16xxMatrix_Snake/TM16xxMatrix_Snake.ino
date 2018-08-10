/*
 * TM16xxMatrix_Snake Example
 * 
 * Example to demonstrate TM16xxMatrix library.
 * Based on Snake.pde example of the MAX72xxPanel library
 * 
 *   Confirmed to work in the following environments:
 *     - ATtiny44A using Arduino IDE 1.8.2 and ATtiny Microcontrolers (8MHz), DIN=9, CLK=10, 3976 bytes flash, 149 bytes RAM ==> 3962/149
 *     - ATtiny44A using Arduino IDE 1.8.2 and ATTinyCore (8MHz, LTO enabled), DIN=9, CLK=10, 3974 bytes flash, 149 bytes RAM
 *     - Arduino Nano and TM1637 5x6 mini-matrix using Arduino IDE 1.8.2, Nano (Old Bootloader), 4126 bytes flash, 149 bytes RAM
 *     - WeMos D1-mini and TM1640 8x8 MatrixLED Shield using Arduino IDE 1.6.10: DIN=D7/13/MOSI, CLK=D5/14/SCK, 249176 bytes flash, 32356 bytes RAM
 *     
**/

#include <TM1640.h>
#include <TM16xxMatrix.h>

//TM1640 module(9, 10);    // DIN=9, CLK=10
TM1640 module(13, 14);    // For ESP8266/WeMos D1-mini: DIN=D7/13/MOSI, CLK=D5/14/SCK
#define MATRIX_NUMCOLUMNS 8
#define MATRIX_NUMROWS 8
TM16xxMatrix matrix(&module, MATRIX_NUMCOLUMNS, MATRIX_NUMROWS);    // TM16xx object, columns, rows

const int pinRandom = A0;

const int wait = 100; // In milliseconds
const int length = 8;

int x[length], y[length];
int ptr, nextPtr;

void setup()
{
  // flash the matrix (using relative slow matrix.setAll)
  matrix.setAll(true);
  delay(200);
  module.setupDisplay(true, 2); // set intensity lower
  delay(400);
  matrix.setAll(false);    // Note: module.clearDisplay() doesn't clear the offscreen bitmap!

  // Reset all variables
  for ( ptr = 0; ptr < length; ptr++ ) {
    x[ptr] = MATRIX_NUMCOLUMNS / 2;
    y[ptr] = MATRIX_NUMROWS / 2;
  }
  nextPtr = 0;

  randomSeed(analogRead(pinRandom)); // Initialize random generator
}

void loop()
{
  // Shift pointer to the next segment
  ptr = nextPtr;
  nextPtr = next(ptr);

  matrix.setPixel(x[ptr], y[ptr], true); // Draw the head of the snake
  //matrix.write(); // Send bitmap to display

  delay(wait);

  if ( ! occupied(nextPtr) ) {
    matrix.setPixel(x[nextPtr], y[nextPtr], false); // Remove the tail of the snake
  }

  for ( int attempt = 0; attempt < 10; attempt++ ) {

    // Jump at random one step up, down, left, or right
    switch ( random(4) ) {
    case 0: x[nextPtr] = constrain(x[ptr] + 1, 0, MATRIX_NUMCOLUMNS - 1); y[nextPtr] = y[ptr]; break;
    case 1: x[nextPtr] = constrain(x[ptr] - 1, 0, MATRIX_NUMCOLUMNS - 1); y[nextPtr] = y[ptr]; break;
    case 2: y[nextPtr] = constrain(y[ptr] + 1, 0, MATRIX_NUMROWS - 1); x[nextPtr] = x[ptr]; break;
    case 3: y[nextPtr] = constrain(y[ptr] - 1, 0, MATRIX_NUMROWS - 1); x[nextPtr] = x[ptr]; break;
    }

    if ( ! occupied(nextPtr) ) {
      break; // The spot is empty, break out the for loop
    }
  }
}

boolean occupied(int ptrA) {
  for ( int ptrB = 0 ; ptrB < length; ptrB++ ) {
    if ( ptrA != ptrB ) {
      if ( equal(ptrA, ptrB) ) {
        return true;
      }
    }
  }

  return false;
}

int next(int ptr) {
  return (ptr + 1) % length;
}

boolean equal(int ptrA, int ptrB) {
  return x[ptrA] == x[ptrB] && y[ptrA] == y[ptrB];
}
