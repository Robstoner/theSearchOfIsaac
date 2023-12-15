/*

*/


// Import required libraries for matrix and LCD
#include <LedControl.h>
#include <LiquidCrystal.h>

#include "Pins.h"

// Initialize matrix
LedControl matControl = LedControl(dinPin, clockPin, loadPin, 1);

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
bool matrixChanged = true;  // Flag to track if the matrix display needs updating

byte pathRow = 4;
byte pathCol = 4;
byte currentRoomX = 0;
byte currentRoomY = 0;
byte adjustedXPos = xPos + matrixSize * currentRoomX;
byte adjustedYPos = yPos + matrixSize * currentRoomY;

// Blink the player
unsigned long lastPlayerBlink = 0;
const int playerBlinkInterval = 100;
bool playerBlinkState = true;


// Game variables
const byte treasure = 2;
const byte modifier = 3;
const byte startingShovels = 5;

const int shovelTimeout = 500;  // Timeout to control the speed of using shovels
const int moveTimeout = 600;   // Timeout control the speed of player movement
unsigned long lastPlayerMovedTime = 0;
unsigned long lastShovelUsedTime = 0;

bool startup = false;
const int startupTime = 2000;
bool playing = false;
bool lostGame = false;

byte difficulty = 1;
int shovels = startingShovels;
unsigned long gameRunningTime = 0;
const int second = 1000;
unsigned long lastSecond = 0;

// Score variables
unsigned int score = 0;
const byte highScoreNr = 3;
unsigned int highScores[highScoreNr] = { 0, 0, 0 };

// Menu variables
bool inMenu = false;

const int firstMenu = 1;
const int lastMenu = 4;
byte currentMenu = 1;
byte currentMenuOption = 1;

const int menuMovementTimeout = 1000;
unsigned long lastMenuMovementTime = 0;

bool showingHighScores = false;
unsigned long startedShowingHighScores = 0;
const int showingHishScoresInterval = 5000;

// Interrupt
const unsigned long debounceInterval = 400;
volatile unsigned long lastButtonPressTime = 0;
volatile bool buttonPressed;

#include "MatrixDrawings.h"
#include "Menu.h"

void joystickButtonInterrupt() {
  static unsigned long buttonPressTime = 0;
  buttonPressTime = micros();

  if (buttonPressTime - lastButtonPressTime > debounceInterval) {
    buttonPressed = true;
  }
}

void setup() {
  // Joystick controls and button interrupt
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(swPin), joystickButtonInterrupt, FALLING);

  // RGB LED
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);

  // Start matrix
  matControl.shutdown(0, false);                 // turn off power saving, enables display
  matControl.setIntensity(0, matrixBrightness);  // sets brightness (0~15 possible values)
  matControl.clearDisplay(0);                    // clear screen

  // Start LCD
  lcd.createChar(0, arrowUp);
  lcd.createChar(1, arrowDown);

  lcd.begin(16, 2);  // define the lcd with 2 rows and 16 columns

  lcd.print(F("   Welcome to   "));
  lcd.setCursor(0, 1);
  lcd.print(F("TheSearchOfIsaac"));

  Serial.begin(serialBaud);

  randomSeed(analogRead(0));

  displayImageInt64(matControl, smiley_face);
}

void loop() {

  if (millis() > startupTime && !startup) {
    inMenu = true;
    startup = true;
    printMenu(currentMenu, currentMenuOption);
  }

  if (startup) {
    if (playing) {
      gameRunning();
    } else {
      gamePaused();
    }
  }
}

void gamePaused() {
  if (showingHighScores) {

    if ((millis() - startedShowingHighScores > showingHishScoresInterval) || buttonPressed) {
      showingHighScores = false;
      printMenu(currentMenu, currentMenuOption);

      inMenu = true;
      buttonPressed = false;
    } else {
      return;
    }
  }

  if (lostGame) {
    // Reset game / back to start menu
    if (buttonPressed) {

      resetGame();
    } else {
      return;
    }
  }

  if (millis() - lastMenuMovementTime > menuMovementTimeout) {
    if (readMenuMovement()) {

      printMenu(currentMenu, currentMenuOption);
    }
  }

  // Choose option in menu
  if (inMenu && buttonPressed) {
    chooseMenuOption();

    buttonPressed = false;
  }
}

void gameRunning() {
  // Check if the player can move (to not take too many inputs too fast)
  if (millis() - lastPlayerMovedTime > moveTimeout) {
    updatePlayerPosition();
    lastPlayerMovedTime = millis();
  }

  showTreasureProximity();

  // Try to use the shovel on the point you are on
  if (buttonPressed) {
    mineTreasure();

    buttonPressed = false;
  }

  if (lostGame) {
    return;
  }

  // Check if the matrix needs to be changed
  if (matrixChanged == true) {
    updateMatrix();
    matrixChanged = false;
  }

  // Update the timer
  if (millis() - lastSecond > second) {
    gameRunningTime++;

    lastSecond = millis();
  }

  printRunningGameInfo();

  blinkPlayer();
}

void printMenu(byte menu, byte option) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("<"));
  lcd.setCursor(15, 0);
  lcd.print(F(">"));

  switch (menu) {
    case 1:
      printStartMenu();

      break;
    case 2:
      printLeaderboardMenu(option);

      break;
    case 3:
      printSettingsMenu(option);

      break;
    case 4:
      printAboutMenu();

      break;
    default:
      break;
  }
}

void chooseMenuOption() {
  if (currentMenu == 1 && currentMenuOption == 1) {
    playing = true;

    startGame();
    inMenu = false;
  }

  if (currentMenu == 2) {
    if (currentMenuOption == 1) {
      showHighScores();
      inMenu = false;
    }
    if (currentMenuOption == 2) {
      resetHighScores();
    }
  }

  if (currentMenu == 3) {
  }

  if (currentMenu == 4) {
  }
}

void mineTreasure() {
  if (millis() - lastShovelUsedTime > shovelTimeout) {
    if (matrix[adjustedXPos][adjustedYPos] == treasure) {
      score += difficulty * modifier;
      matrix[adjustedXPos][adjustedYPos] = 0;
    } else {
      shovels--;

      if (shovels < 0) {
        lostGameFunction();
      }
    }

    lastShovelUsedTime = millis();
  }
}

void startGame() {
  generateRooms();
  updateMatrix();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print((String) "Shvl:" + shovels + " Score:" + score);
  lcd.setCursor(0, 1);
  lcd.print((String) "Time:" + gameRunningTime);

  lastSecond = millis();
}

void resetGame() {
  lostGame = false;
  displayImageInt64(matControl, smiley_face);

  buttonPressed = false;

  inMenu = true;
  shovels = startingShovels;
  gameRunningTime = 0;
  score = 0;
  xPos = 4;
  yPos = 4;
  adjustedXPos = xPos + matrixSize * currentRoomX;
  adjustedYPos = yPos + matrixSize * currentRoomY;

  currentMenu = firstMenu;
  currentMenuOption = 1;
  printMenu(currentMenu, currentMenuOption);
}

void lostGameFunction() {
  playing = false;
  lostGame = true;

  displayImageInt64(matControl, sad_face);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("You lost. Score:"));
  lcd.setCursor(0, 1);
  lcd.print(score);

  // Check if high score
  if (checkHighScoreAndSave()) {
    lcd.print(F(" HIGH SCORE"));
    displayImageInt64(matControl, cup);
  }
}

bool checkHighScoreAndSave() {
  for (int i = 0; i < highScoreNr; ++i) {
    Serial.println((String) "||" + i + " " + highScores[i]);
    if (score > highScores[i]) {
      for (int j = highScoreNr - 1; j > i; --j) {
        highScores[j] = highScores[j - 1];
      }

      highScores[i] = score;

      return true;
    }
  }

  return false;
}

void resetHighScores() {
  for (int i = 0; i < highScoreNr; ++i) {
    highScores[i] = 0;
  }
}

void showHighScores() {
  showingHighScores = true;
  startedShowingHighScores = millis();
  lcd.clear();


  lcd.setCursor(0, 1);
  for (int i = 0; i < highScoreNr; ++i) {
    lcd.print(highScores[i]);
    lcd.print("    ");
  }
}

/*
 * green - on treasure
 * blue - 1 off from treasure
 * red - 2+ off treasure
*/
void showTreasureProximity() {
  byte treasureProximity = checkTreasureProximity();

  if (treasureProximity == 0) {
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(blueLedPin, LOW);
    digitalWrite(redLedPin, LOW);
  } else if (treasureProximity == 1) {
    digitalWrite(greenLedPin, LOW);
    digitalWrite(blueLedPin, HIGH);
    digitalWrite(redLedPin, LOW);
  } else {
    digitalWrite(greenLedPin, LOW);
    digitalWrite(blueLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
  }
}

/* 
 * 0 - On treasure
 * 1 - 1 away from treasure
 * 2 - 2 or more away from treasure
*/
byte checkTreasureProximity() {
  // On treasure
  if (matrix[adjustedXPos][adjustedYPos] == 2) {
    return 0;
  }
  // Within 1 block from treasure
  if (matrix[adjustedXPos][adjustedYPos + 1] == 2 || matrix[adjustedXPos + 1][adjustedYPos + 1] == 2 || matrix[adjustedXPos + 1][adjustedYPos] == 2 || matrix[adjustedXPos + 1][adjustedYPos - 1] == 2 || matrix[adjustedXPos][adjustedYPos - 1] == 2 || matrix[adjustedXPos - 1][adjustedYPos - 1] == 2 || matrix[adjustedXPos - 1][adjustedYPos] == 2 || matrix[adjustedXPos - 1][adjustedYPos + 1] == 2) {
    return 1;
  }

  return 2;
}

void updatePlayerPosition() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

  // Store the last positions of the LED
  xLastPos = xPos;
  yLastPos = yPos;

  // Update xPos based on joystick movement (X-axis)
  // Down
  if (xValue < minThreshold) {
    if (xPos > 0) {
      xPos--;
    }
  }
  // Up
  if (xValue > maxThreshold) {
    if (xPos < matrixSize - 1) {
      xPos++;
    }
  }

  // Update yPos based on joystick movement (Y-axis)
  // Left
  if (yValue > maxThreshold) {
    if (yPos > 0) {
      yPos--;
    }
  }
  // Right
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
      checkMovedRoom();

      adjustedXPos = xPos + matrixSize * currentRoomX;
      adjustedYPos = yPos + matrixSize * currentRoomY;

      matrixChanged = true;
    }
  }
}

void checkMovedRoom() {
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

void generateRooms() {
  for (int row = 0; row < matrixSize * roomsX; row++) {
    for (int col = 0; col < matrixSize * roomsY; col++) {
      if ((row != 0 && col != 0 && row != (matrixSize * roomsX - 1) && col != (matrixSize * roomsY - 1))
          && ((row % matrixSize) == pathRow || (col % matrixSize) == pathCol)) {
        // Ignore path between rooms so player can at least move between them
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