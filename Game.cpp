// Game.cpp //

#include "Game.h"
#include "Glyphs.h"
#include "Utils.h"

// Piece IDs
const int PIECE_I = 0;
const int PIECE_J = 1;
const int PIECE_L = 2;
const int PIECE_O = 3;
const int PIECE_S = 4;
const int PIECE_T = 5;
const int PIECE_Z = 6;
const int UPDATE_SPACING = 1000; // Time between each update in ms
const int SPAWN_LOCATION_X = 2;

// Prototypes for hidden functions
void spawnNewPiece();
void movePiece();
void handleGameInput();
void checkCollisionAndUpdate();
boolean isCollision(boolean tmpPiece[4][4], int tmpPosX, int tmpPoxY);
void placePiece();
void checkForFullRows();

// Areal piece layout
boolean PIECES[7][4][4] = { //[piece][y][x]
{
  {0, 0, 0, 0},
  {1, 1, 1, 1},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
},{
  {0, 0, 0, 0},
  {1, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 0, 0}
},{
  {0, 0, 0, 0},
  {0, 1, 1, 1},
  {0, 1, 0, 0},
  {0, 0, 0, 0}
},{
  {0, 0, 0, 0},
  {0, 1, 1, 0},
  {0, 1, 1, 0},
  {0, 0, 0, 0}
},{
  {0, 0, 0, 0},
  {0, 1, 1, 0},
  {1, 1, 0, 0},
  {0, 0, 0, 0}
},{
  {0, 0, 0, 0},
  {1, 1, 1, 0},
  {0, 1, 0, 0},
  {0, 0, 0, 0}
},{
  {0, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 0, 0}
  
}
};

byte area[8]; // Internal screen of placed pieces (origin is in top, left corner)
byte* display; // Main internal screen for rendering/drawing to external screen
boolean waitForGameOver = false; // Real game over, for waiting a time unit before exiting state
boolean gameOver = false; // If the game is over
unsigned long lastUpdate = 0; // Last time piece was moved by gravity
boolean piece[4][4]; // Active piece
boolean activePiece = false; // If a piece is active
int posX, posY; // Location of active piece
InputData newInputData; // Storage for unhandled user input

// Initiate current game (called from loop) //
void initGame(byte* screen) {
  display = screen;
  for (int i = 0; i < 8; i++)
    area[i] = 0;
  waitForGameOver = false;
  gameOver = false;
  lastUpdate = 0;
  activePiece = false;
}

// Update the game (called from loop) //
void updateGame(InputData inputData) {
  // Save new user input
  if (inputData.rotate)
    newInputData.rotate = true;
  if (inputData.left)
    newInputData.left = true;
  if (inputData.right)
    newInputData.right = true;
  if (inputData.drop)
    newInputData.drop = true;
  
  if (waitForGameOver) {
    if (millis() > lastUpdate + UPDATE_SPACING)
      gameOver = true;
  } else if (activePiece) {
    movePiece();
    handleGameInput();
    checkCollisionAndUpdate();
  } else {
    spawnNewPiece();
    checkCollisionAndUpdate();
  }
  
  // Draw environment and active piece to main internal screen
  for (int i = 0; i < 8; i++)
    display[i] = area[i];
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      int absX = posX + x;
      int absY = posY + y;
      if (absX > -1 && absX < 8 && absY > -1 && absY < 8 && piece[y][x])
        setPixel(display, absX, absY, true);
    }
  }
}

// Spawn a new piece //
void spawnNewPiece() {
  int newPiece;
  switch(random(7)) {
    case 0:
      newPiece = PIECE_I;
      break;
    case 1:
      newPiece = PIECE_J;
      break;
    case 2:
      newPiece = PIECE_L;
      break;
    case 3:
      newPiece = PIECE_O;
      break;
    case 4:
      newPiece = PIECE_S;
      break;
    case 5:
      newPiece = PIECE_T;
      break;
    case 6:
      newPiece = PIECE_Z;
      break;
  }
  
  copy4x4(piece, PIECES[newPiece]);
  
  // Find lowest point of piece, for calculating where it should spawn
  int lowestOccupiedRow = -1;
  for (int y = 3; y > -1 && lowestOccupiedRow < 0; y--) {
    for (int x = 0; x < 4 && lowestOccupiedRow < 0; x++) {
      if (piece[y][x])
        lowestOccupiedRow = y;
    }
  }
  
  posX = SPAWN_LOCATION_X;
  posY = -lowestOccupiedRow - 1;
  activePiece = true;
  
  Serial.print("Spawned new piece at x=");
  Serial.print(posX);
  Serial.print(" y=");
  Serial.println(posY);
}

// Move active piece //
void movePiece() {
  unsigned long time = millis();
  if (time > lastUpdate + UPDATE_SPACING) {
    lastUpdate = time;
    posY++;
  }
}

// Handle user input //
void handleGameInput() {
  if (newInputData.rotate) {
    // FIXME: Rotates around center of array, not center of piece.
    newInputData.rotate = false;
    boolean tmpPiece[4][4];
    rotate4x4_270(tmpPiece, piece);
    if (!isCollision(tmpPiece, posX, posY))
      copy4x4(piece, tmpPiece);
  } if (newInputData.left) {
    newInputData.left = false;
    if (!isCollision(piece, posX - 1, posY))
      posX--;
  } if (newInputData.right) {
    newInputData.right = false;
    if (!isCollision(piece, posX + 1, posY))
      posX++;
  } if (newInputData.drop) {
    newInputData.drop = false;
    if (!isCollision(piece, posX, posY + 1))
      posY++;
  }
}

// Place active piece if standing on something, and check for full rows //
void checkCollisionAndUpdate() {
  if (isCollision(piece, posX, posY + 1)) {
    placePiece();
    checkForFullRows();
  }
}

// Check if specified piece collides in specified location //
boolean isCollision(boolean tmpPiece[4][4], int tmpPosX, int tmpPoxY) {
  boolean collision = false;
  for (int x = 0; x < 4 && !collision; x++) {
    for (int y = 0; y < 4 && !collision; y++) {
      if (tmpPiece[y][x]) {
        int absX = tmpPosX + x;
        int absY = tmpPoxY + y;
        //Firstly, check if outside of screen (not entry side)
        if (absX < 0 || absX > 7 || absY > 7)
          collision = true;
        //Secondly, check if overlapping environment
        if (getPixel(area, absX, absY))
          collision = true;
      }
    }
  }

  return collision;
}

// Place active piece //
void placePiece() {
  Serial.print("Placing piece at x=");
  Serial.print(posX);
  Serial.print(" y=");
  Serial.println(posY);
  
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      int absX = posX + x;
      int absY = posY + y;
      if (absY < 0)
        waitForGameOver = true;
      else if (piece[y][x])
        setPixel(area, absX, absY, true);
    }
  }
  activePiece = false;
}

// Check for and remove full rows //
void checkForFullRows() {
  boolean runAgain = true;
  while(runAgain) {
    runAgain = false;
    boolean emptyTiles = true;
    for (int y = 0; y < 8 && emptyTiles; y++) {
      emptyTiles = false;
      for (int x = 0; x < 8 && !emptyTiles; x++) {
        if (!getPixel(area, x, y))
          emptyTiles = true;
      }
      if (!emptyTiles) {
        for (int y2 = y - 1; y2 > -1; y2--) {
          for (int x = 0; x < 8; x++)
            setPixel(area, x, y2 + 1, getPixel(area, x, y2));
        }
        runAgain = true;
        Serial.print("Removed row y=");
        Serial.println(y);
      }
    }
  }
}

// If the game is over //
boolean isGameOver() {
  return gameOver;
}
