// Utils.h //

#ifndef Utils_h
#define Utils_h

#include <Arduino.h>

boolean getPixel(byte columns[], int x, int y);
void setPixel(byte columns[], int x, int y, boolean state);
void rotate8_270(byte image[], byte glyph[]);
void rotate4x4_270(boolean dst[4][4], boolean src[4][4]);
void copy8(byte dst[8], byte src[8]);
void copy4x4(boolean dst[4][4], boolean src[4][4]);

#endif // Utils_h
