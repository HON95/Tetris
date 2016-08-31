// MaxMatrix8x8.cpp //

#include "MaxMatrix8x8.h"

// Prototypes for hidden functions
void putByte(byte data);

byte last_columns[8]; // Last updates to the columns, for checking if an update is necessary

// Initialize the LED matrix //
void initMatrix() {
  pinMode(PIN_MAX_DOUT, OUTPUT);
  pinMode(PIN_MAX_CS, OUTPUT);
  pinMode(PIN_MAX_CLK, OUTPUT);
  
  maxOne(MAX7219_REG_SCAN_LIMIT, 0x07);
  maxOne(MAX7219_REG_DECODE_MODE, 0x00);
  maxOne(MAX7219_REG_SHUTDOWN, 0x01);
  maxOne(MAX7219_REG_DISPLAY_TEST, 0x00);
  maxOne(MAX7219_REG_INTENSITY, DEFAULT_INTENSITY & 0x0f);
  
  clearMatrix();
}

// Set LED intensity for matrix //
void setMatrixIntensity(int intensity) {
  maxOne(MAX7219_REG_INTENSITY, intensity & 0x0f);
}

// Clear the matrix //
void clearMatrix() {
  for (int i = 0; i < 8; i++) {
    last_columns[i] = 0;
    maxOne(i + 1, 0);
  }
}

// Send 8-byte image to matrix //
void setMatrixImage(byte columns[]) {
  for (int x = 0; x < 8; x++) {
    if (columns[x] != last_columns[x]) {
      last_columns[x] = columns[x];
      maxOne(x + 1, columns[x]);
    }
  }
}

// Set a column of the matrix //
void maxOne(byte reg, byte col) {
  digitalWrite(PIN_MAX_CS, LOW);
  putByte(reg);
  putByte(col);
  digitalWrite(PIN_MAX_CS, HIGH);
}

// Send byte to matrix //
void putByte(byte data) {
  byte i = 8;
  byte mask;
  while(i > 0) {
    mask = 0x01 << (i - 1);
    digitalWrite(PIN_MAX_CLK, LOW);
    if (data & mask)
      digitalWrite(PIN_MAX_DOUT, HIGH);
    else
      digitalWrite(PIN_MAX_DOUT, LOW);
    digitalWrite(PIN_MAX_CLK, HIGH);
    --i;
  }
}
