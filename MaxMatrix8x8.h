// MaxMatrix8x8.h //

#ifndef MaxMatrix8x8_h
#define MaxMatrix8x8_h

#include <Arduino.h>

const int PIN_MAX_DOUT = 2; // Data
const int PIN_MAX_CS = 3; // Load
const int PIN_MAX_CLK = 4; // Clock

const byte MAX7219_REG_NOOP         = 0x00;
const byte MAX7219_REG_DIGIT0       = 0x01;
const byte MAX7219_REG_DIGIT1       = 0x02;
const byte MAX7219_REG_DIGIT2       = 0x03;
const byte MAX7219_REG_DIGIT3       = 0x04;
const byte MAX7219_REG_DIGIT4       = 0x05;
const byte MAX7219_REG_DIGIT5       = 0x06;
const byte MAX7219_REG_DIGIT6       = 0x07;
const byte MAX7219_REG_DIGIT7       = 0x08;
const byte MAX7219_REG_DECODE_MODE  = 0x09;
const byte MAX7219_REG_INTENSITY    = 0x0A;
const byte MAX7219_REG_SCAN_LIMIT   = 0x0B;
const byte MAX7219_REG_SHUTDOWN     = 0x0C;
const byte MAX7219_REG_DISPLAY_TEST = 0x0F;

const byte DEFAULT_INTENSITY = 0x01;

void initMatrix();
void setNatrixIntensity(byte intensity);
void clearMatrix();
void setMatrixImage(byte columns[]);
void maxOne(byte reg, byte col);

#endif // MaxMatrix8x8_h
