// Tetris game by Havard ON, 2014 //

#include "IRremote.h"
#include "MaxMatrix8x8.h"
#include "Glyphs.h"
#include "Utils.h"
#include "Game.h"

// Pinout
const int PIN_IR = 11;

// States
const int STATE_OFF = 0;
const int STATE_INTRO = 1;
const int STATE_GAME = 2;
const int STATE_GAME_OVER = 3;

// State names
const String STATE_NAMES[] = {
  "STATE_OFF",
  "STATE_INTRO",
  "STATE_GAME",
  "STATE_GAME_OVER"
};

// IR signals from remote control
const unsigned long SIGNAL_POWER_A = 0x00300C;
const unsigned long SIGNAL_POWER_B = 0x01300C;
const unsigned long SIGNAL_MODE_A = 0x003042;
const unsigned long SIGNAL_MODE_B = 0x013042;
const unsigned long SIGNAL_MUTE_A = 0x00000D;
const unsigned long SIGNAL_MUTE_B = 0x00080D;
const unsigned long SIGNAL_ROTATE_A = 0x003058;
const unsigned long SIGNAL_ROTATE_B = 0x013058;
const unsigned long SIGNAL_LEFT_A = 0x00305A;
const unsigned long SIGNAL_LEFT_B = 0x01305A;
const unsigned long SIGNAL_RIGHT_A = 0x00305B;
const unsigned long SIGNAL_RIGHT_B = 0x01305B;
const unsigned long SIGNAL_DROP_A = 0x003059;
const unsigned long SIGNAL_DROP_B = 0x013059;

// Scrolling text IDs
const int TEXT_INTRO = 0;
const int TEXT_GAME_OVER = 1;

// Other
const int TEXT_MOVE_TIME = 125; // Glyph movement in ms per pixel
const int TEXT_MOVE_SPACE = 0; // Pixels between glyphs
const int INPUT_DECODE_TYPE = RC6; // IR remote control decode type
const int INPUT_SPACING = 150; // Time to wait before accepting input signals again in ms
const int START_STATE = STATE_INTRO; // State to start game with

// Scrolling texts
byte *textIntro[] = {
  GLYPH_T,
  GLYPH_E,
  GLYPH_T,
  GLYPH_R,
  GLYPH_I,
  GLYPH_S,
  0
};
byte *textGameOver[] = {
  GLYPH_G,
  GLYPH_A,
  GLYPH_M,
  GLYPH_E,
  GLYPH_SPACE,
  GLYPH_O,
  GLYPH_V,
  GLYPH_E,
  GLYPH_R,
  GLYPH_SPACE,
  0
};

IRrecv irrecv(PIN_IR); // IR receiver
boolean running = false; // If game is running
int state; // Current game state
byte screen[8]; // Data on internal screen
unsigned long lastInputSignalTime = 0; // Time of reception of last input signal
unsigned long lastInputSignal = 0; // Last received, non-repetetive input signal
boolean mute; // If music is muted
int textCurrent; // Current showing scrolling text
unsigned long textLastMove; // Time of last scrolling text movement
int textPos; // Scrolling text offset
boolean textFinished; // If scrolling text is finished
boolean showingLogo; // If logo is showing

// Initialize systems (called by Arduino) //
void setup() {
  randomSeed(analogRead(0));
  Serial.begin(9600);
  initMatrix();
  irrecv.enableIRIn();
}

// Starts the game, returns when game is over (called by Arduino) //
void loop() {
  initInstance();
  while(running) {
    InputData inputData = handleInput();
    switch (state) {
      case STATE_OFF: {
        break;
      } case STATE_INTRO: {
        if (!textFinished) {
          showText();
        } else if (!showingLogo) {
          showingLogo = true;
          rotate8_270(screen, GLYPH_LOGO);
        }
        break;
      } case STATE_GAME: {
        updateGame(inputData);
        if (isGameOver())
          switchState(STATE_GAME_OVER);
        break;
      } case STATE_GAME_OVER: {
         showText();
         if (textFinished)
           stopGame();
         break;
      } default: {
        break;
      }
    }
    updateScreen();
    delay(5);
  }
  shutdownGame();
}

// Initialized an instance of the game (called from loop) //
void initInstance() {
  Serial.println("Initiating game");
  running = true;
  switchState(START_STATE);
}

// Shut down game (called from loop) //
void shutdownGame() {
  Serial.println("Shutting down game");
}

// Publish internal screen to external screen //
void updateScreen() {
  setMatrixImage(screen);
}

// Stop the game next game loop //
void stopGame() {
  running = false;
}

// Switch game state //
void switchState(int nextState) {
  state = nextState;
  Serial.print("Changing state to: ");
  Serial.println(STATE_NAMES[state]);
  clearScreen();
  switch(nextState) {
    case STATE_OFF: {
      break;
    } case STATE_INTRO: {
      showingLogo = false;
      startShowingText(TEXT_INTRO);
      break;
    } case STATE_GAME: {
      initGame(screen);
      break;
    } case STATE_GAME_OVER: {
      startShowingText(TEXT_GAME_OVER);
      break;
    } default: {
      break;
    }
  }
}

// Handle user input (called from loop) //
InputData handleInput() {
  InputData data;
  decode_results results;
  if (irrecv.decode(&results)) {
    irrecv.resume();
    unsigned long time = millis();
    if (results.decode_type == INPUT_DECODE_TYPE && time > lastInputSignalTime + INPUT_SPACING) {
      lastInputSignalTime = time;
      unsigned long signal = (results.value != REPEAT ? results.value : lastInputSignal);
      lastInputSignal = signal;
      //Serial.println(signal, HEX);
      switch (signal) {
        case SIGNAL_POWER_A: case SIGNAL_POWER_B: {
          if (state == STATE_OFF)
            switchState(STATE_INTRO);
          else
            switchState(STATE_OFF);
          break;
        } case SIGNAL_MODE_A: case SIGNAL_MODE_B: {
          int nextState = state + 1;
          if (nextState > STATE_GAME_OVER)
            nextState = STATE_INTRO;
          switchState(nextState);
          break;
        } case SIGNAL_MUTE_A: case SIGNAL_MUTE_B: {
          mute = !mute;
          Serial.print("Mute=");
          Serial.println(mute);
          break;
        } case SIGNAL_ROTATE_A: case SIGNAL_ROTATE_B: {
          data.rotate = true;
          break;
        } case SIGNAL_LEFT_A: case SIGNAL_LEFT_B: {
          data.left = true;
          break;
        } case SIGNAL_RIGHT_A: case SIGNAL_RIGHT_B: {
          data.right = true;
          break;
        } case SIGNAL_DROP_A: case SIGNAL_DROP_B: {
          data.drop = true;
          break;
        } default: { 
         break;
        }
      }
    }
  }
  return data;
}

// Prepare for showing scrolling text //
void startShowingText(int text) {
  textCurrent = text;
  textLastMove = millis();
  textPos = 0;
  textFinished = false;
}

// Update scrolling text //
void showText() {
  unsigned long time = millis();
  if ((textPos == 0 && time > textLastMove + 5 * TEXT_MOVE_TIME) || (textPos > 0 && time > textLastMove + TEXT_MOVE_TIME)) {
    textLastMove = time;
    textPos++;
  }
  
  int charNumber = textPos / (8 + TEXT_MOVE_SPACE);
  int charPos = textPos % (8 + TEXT_MOVE_SPACE);
  if (!textFinished && ((textCurrent == TEXT_INTRO && textIntro[charNumber]) || (textCurrent == TEXT_GAME_OVER && textGameOver[charNumber]))) {
    byte char1[8];
    byte char2[8];
    if (textCurrent == TEXT_INTRO) {
      rotate8_270(char1, textIntro[charNumber]);
      rotate8_270(char2, (textIntro[charNumber + 1] ? textIntro[charNumber + 1] : GLYPH_SPACE));
    } else if (textCurrent == TEXT_GAME_OVER) {
      rotate8_270(char1, textGameOver[charNumber]);
      rotate8_270(char2, (textGameOver[charNumber + 1] ? textGameOver[charNumber + 1] : GLYPH_SPACE));
    }
    
    for (int x = 0; x < 8; x++) {
      if (charPos + x < 8) {
        screen[x] = char1[x + charPos];
      } else if (charPos + x < 8 + TEXT_MOVE_SPACE) {
        screen[x] = 0;
      } else {
        screen[x] = char2[x - (TEXT_MOVE_SPACE + 8 - charPos)];
      }
    }
  } else {
    textFinished = true;
  }
}

// Clear the screen //
void clearScreen() {
  for (int i = 0; i < 8; i++)
    screen[i] = 0;
}
