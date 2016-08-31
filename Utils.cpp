// Utils.cpp //

#include "Utils.h"

// Get a pixel from an image //
boolean getPixel(byte columns[], int x, int y) {
  return (columns[x] >> y) & 1;
}

// Set a pixel in an image //
void setPixel(byte columns[], int x, int y, boolean state) {
  if (state)
    columns[x] = columns[x] | (1 << y);
  else
    columns[x] = columns[x] & (0xFF - (1 << y));
}

// Rotate a 8-byte image 270 degrees //
void rotate8_270(byte image[], byte glyph[]) {
  for (int x = 0; x < 8; x++) {
    int column = 0;
    for (int y = 0; y < 8; y++)
      column = column | (((glyph[y] >> (7 - x)) & 1) << y);
    image[x] = column;
  }
}

// Rotate a 4x4 image 270 degrees //
void rotate4x4_270(boolean dst[4][4], boolean src[4][4]) {
  for (int x = 0; x < 4; x++) {
      for (int y = 0; y < 4; y++)
        dst[y][3 - x] = src[x][y];
    }
}

// Copy a 8-byte image //
void copy8(byte dst[8], byte src[8]) {
  for (int i = 0; i < 8; i++)
    dst[i] = src[i];
}

// Copy a 4x4 image //
void copy4x4(boolean dst[4][4], boolean src[4][4]) {
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++)
      dst[y][x] = src[y][x];
  }
}
