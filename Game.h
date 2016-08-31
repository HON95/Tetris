// Game.h //

#ifndef Game_h
#define Game_h

#include <Arduino.h>

typedef struct InputDataStruct {
  boolean rotate;
  boolean left;
  boolean right;
  boolean drop;
  InputDataStruct():rotate(0), left(0), right(0), drop(0) {}
} InputData;

void initGame(byte *screen);
void updateGame(InputData inputData);
boolean isGameOver();

#endif // Game_h
