/*

*/


// Import required libraries for matrix and LCD
#include <LedControl.h>
#include <LiquidCrystal.h>

#include "MatrixDrawings.h"
#include "Pins.h"

// Initialize matrix
LedControl matControl = LedControl(dinPin, clockPin, loadPin, 1);

// Initialize LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Joystick variables
byte xPos = 4;
byte yPos = 4;
byte xLastPos = 4;
byte yLastPos = 4;
const int minThreshold = 200;
const int maxThreshold = 600;

// Matrix variables
byte matrixBrightness = 2;
const byte matrixSize = 8;  // nr of rows and columns the matrix has
const byte roomsX = 2;      // nr of rows of rooms
const byte roomsY = 2;      // nr of cols of rooms

// Matrix representation
byte matrix[matrixSize * roomsX][matrixSize * roomsY];

const byte moveInterval = 100;     // Timing variable to control the speed of LED movement
unsigned long long lastMoved = 0;  // Tracks the last time the LED moved
bool matrixChanged = true;         // Flag to track if the matrix display needs updating

// LCD Variables


// Game variables
bool playing = false;
const int startupTime = 2000;
bool startGame = false;
bool inMenu = false;

// Map variables
byte pathRow = 4;
byte pathCol = 4;
byte currentRoomX = 0;
byte currentRoomY = 0;

// Blink the player
unsigned long lastPlayerBlink = 0;
const int playerBlinkInterval = 100;
bool playerBlinkState = true;

// Interrupt
const unsigned long debounceInterval = 200;
volatile unsigned long lastButtonPressTime = 0;
volatile bool buttonPressed;

void joystickButtonInterrupt() {
  static unsigned long buttonPressTime = 0;
  buttonPressTime = micros();

  if (buttonPressTime - lastButtonPressTime > debounceInterval) {
    buttonPressed = true;
  }
}

void setup() {
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(swPin), joystickButtonInterrupt, FALLING);

  // Start matrix
  matControl.shutdown(0, false);                 // turn off power saving, enables display
  matControl.setIntensity(0, matrixBrightness);  // sets brightness (0~15 possible values)
  matControl.clearDisplay(0);                    // clear screen

  // Start LCD
  lcd.begin(16, 2);  // define the lcd with 2 rows and 16 columns

  lcd.print(F("   Welcome to   "));
  lcd.setCursor(0, 1);
  lcd.print(F("TheSearchOfIsaac"));

  Serial.begin(serialBaud);

  randomSeed(analogRead(0));

  displayImageInt64(matControl, smiley_face);

  // generateRooms();

  // updateMatrix();
}

void loop() {

  if (millis() > startupTime && !startGame) {
    startGame = true;
  }

  if (playing) {
    gameRunning();
  } else {
    gamePaused();
  }
}

void gamePaused() {
  if (startGame && !inMenu) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F(" < Start Game   "));
    lcd.setCursor(0, 1);
    lcd.print(F("   Enter Menu  "));
    inMenu = true;
  }

  if (inMenu && buttonPressed) {
    playing = true;

    generateRooms();

    inMenu = false;
    buttonPressed = false;
  }
}

void gameRunning() {
  // Check if the player can move (to not run the game too fast)
    if (millis() - lastMoved > moveInterval) {
      updatePlayerPosition();
      lastMoved = millis();
    }

    // Check if the matrix needs to be changed
    if (matrixChanged == true) {
      updateMatrix();
      matrixChanged = false;
    }

    blinkPlayer();
}

void generateRooms() {
  for (int row = 0; row < matrixSize * roomsX; row++) {
    for (int col = 0; col < matrixSize * roomsY; col++) {
      if ((row != 0 && col != 0 && row != (matrixSize * roomsX - 1) && col != (matrixSize * roomsY - 1))
          && ((row % matrixSize) == pathRow || (col % matrixSize) == pathCol)) {
        // Ignore path between rooms so player can at least move between them
        Serial.print((String)row + "  " + col + " | ");
        matrix[row][col] = 0;
      } else if ((row % matrixSize) == (matrixSize - 1) || (col % matrixSize) == (matrixSize - 1)
                 || row % matrixSize == 0 || col % matrixSize == 0) {
        // Ignore first and last row and col in each room as they are walls
        matrix[row][col] = 1;
      } else {
        byte addition = random(3);

        matrix[row][col] = addition;
      }
    }
  }
}

void blinkPlayer() {
  if (millis() - lastPlayerBlink > playerBlinkInterval) {
    playerBlinkState = !playerBlinkState;
    matControl.setLed(0, xPos, yPos, playerBlinkState);

    lastPlayerBlink = millis();
  }
}

void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      bool value = matrix[row + matrixSize * currentRoomX][col + matrixSize * currentRoomY] % 2;
      matControl.setLed(0, row, col, value);  // set each led individually
    }
  }
}

void updatePlayerPosition() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

  // Store the last positions of the LED
  xLastPos = xPos;
  yLastPos = yPos;

  // Update xPos based on joystick movement (X-axis)
  if (xValue < minThreshold) {
    if (xPos > 0) {
      xPos--;
    }
  }
  if (xValue > maxThreshold) {
    if (xPos < matrixSize - 1) {
      xPos++;
    }
  }

  // Update yPos based on joystick movement (Y-axis)
  if (yValue > maxThreshold) {
    if (yPos > 0) {
      yPos--;
    }
  }
  if (yValue < minThreshold) {
    if (yPos < matrixSize - 1) {
      yPos++;
    }
  }

  // Check if the position has changed and update the matrix if necessary
  if (xPos != xLastPos || yPos != yLastPos) {

    // Check if the new position is a wall and revert it
    if (matrix[xPos + matrixSize * currentRoomX][yPos + matrixSize * currentRoomY] == 1) {
      xPos = xLastPos;
      yPos = yLastPos;
    } else {

      // Check if player is going to next room
      moveRoom();

      matrixChanged = true;
    }
  }
}

void moveRoom() {
  // Right
  if (xPos == (pathRow) && yPos == matrixSize - 1) {
    currentRoomY++;
    yPos = 1;
  }
  // Left
  if (xPos == (pathRow) && yPos == 0) {
    currentRoomY--;
    yPos = matrixSize - 2;
  }
  // Up
  if (yPos == (pathCol) && xPos == 0) {
    currentRoomX--;
    xPos = matrixSize - 2;
  }
  // Down
  if (yPos == (pathCol) && xPos == matrixSize - 1) {
    currentRoomX++;
    xPos = 1;
  }
}