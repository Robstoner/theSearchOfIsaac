/*

*/


// Import required libraries for matrix and LCD
#include <LedControl.h>
#include <LiquidCrystal.h>

const int serialBaud = 9600;

// Define pins used for matrix
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;

// Define pins used for LCD
const byte rs = 4;
const byte en = 5;
const byte d4 = 6;
const byte d5 = 7;
const byte d6 = 8;
const byte d7 = 9;

// Define pins used for joystick
const int xPin = A0;
const int yPin = A1;

// Initialize matrix
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

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
byte matrix[matrixSize * roomsX][matrixSize * roomsY] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

const byte moveInterval = 100;     // Timing variable to control the speed of LED movement
unsigned long long lastMoved = 0;  // Tracks the last time the LED moved
bool matrixChanged = true;         // Flag to track if the matrix display needs updating

// LCD Variables


// Map variables
byte pathRow = 4;
byte pathCol = 4;
byte currentRoomX = 0;
byte currentRoomY = 0;

// Blink the player
unsigned long lastPlayerBlink = 0;
const int playerBlinkInterval = 100;
bool playerBlinkState = true;

void setup() {

  // Start matrix
  lc.shutdown(0, false);                 // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness);  // sets brightness (0~15 possible values)
  lc.clearDisplay(0);                    // clear screen

  // Start LCD
  lcd.begin(16, 2);  // define the lcd with 2 rows and 16 columns

  lcd.print(F("     Welcome    "));

  Serial.begin(serialBaud);

  randomSeed(analogRead(0));

  generateRooms();

  updateMatrix();
}

void loop() {

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
    lc.setLed(0, xPos, yPos, playerBlinkState);

    lastPlayerBlink = millis();
  }
}

void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      bool value = matrix[row + matrixSize * currentRoomX][col + matrixSize * currentRoomY] % 2;
      lc.setLed(0, row, col, value);  // set each led individually
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