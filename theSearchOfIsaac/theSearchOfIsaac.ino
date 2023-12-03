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
const byte rs = 2;
const byte en = 3;
const byte d4 = 4;
const byte d5 = 5;
const byte d6 = 6;
const byte d7 = 7;

// Initialize matrix
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

// Initialize LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// Matrix variables
byte matrixBrightness = 2;
const byte matrixSize = 8; // nr of rows and columns the matrix has

// Matrix representation
byte matrix[matrixSize][matrixSize] = {
  { 1, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 1, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 1 }
};

// LCD Variables


void setup() {

  // Start matrix
  lc.shutdown(0, false);                 // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness);  // sets brightness (0~15 possible values)
  lc.clearDisplay(0);                    // clear screen

  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row][col]);  // set each led individually
    }
  }

  // Start LCD
  lcd.begin(16, 2); // define the lcd with 2 rows and 16 columns

  lcd.write('A');

  Serial.begin(serialBaud);
}

void loop() {

}
