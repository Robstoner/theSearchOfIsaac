byte arrowUp[8] = {
	0b00000,
	0b00100,
	0b01110,
	0b10101,
	0b00100,
	0b00100,
	0b00100,
	0b00000
};

byte arrowDown[8] = {
	0b00000,
	0b00100,
	0b00100,
	0b00100,
	0b10101,
	0b01110,
	0b00100,
	0b00000
};

// Initialize LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void printRunningGameInfo() {
  lcd.setCursor(5, 0);
  lcd.print(shovels);
  lcd.setCursor(13, 0);
  lcd.print(score);
  lcd.setCursor(5, 1);
  lcd.print(gameRunningTime);
}

// Print arrow up/down for menu
void printArrows(byte option, byte minOption, byte maxOption) {
  if (option > minOption) {
    lcd.setCursor(15, 1);
    lcd.write((uint8_t)0);
  }
  if (option < maxOption) {
    lcd.setCursor(0, 1);
    lcd.write((uint8_t)1);
  }
}

// Start Menu
void printStartMenu() {
  displayImageInt64(matControl, smiley_face);
  lcd.setCursor(1, 0);
  lcd.print(F(" Next  Prev "));
  lcd.setCursor(0, 1);
  lcd.print(F("   Start Game   "));
}

// Leaderboard
void printLeaderboardMenu(byte option) {
  displayImageInt64(matControl, cup);
  lcd.setCursor(1, 0);
  lcd.print(F(" Leaderboard  "));

  printArrows(option, 1, 2);

  lcd.setCursor(1, 1);
  switch (option) {
    case 1:
      lcd.print(F("See top player"));
      break;
    case 2:
      lcd.print(F("    Reset     "));
      break;
    default:
      break;
  }
}

// Settings
void printSettingsMenu(int option) {
  displayImageInt64(matControl, wrench);
  lcd.setCursor(1, 0);
  lcd.print(F("   Settings   "));

  printArrows(option, 1, 3);

  lcd.setCursor(1, 1);
  switch (option) {
    case 1:
      lcd.print(F("LCD brightness"));
      break;
    case 2:
      lcd.print(F("Mat brightness"));
      break;
    case 3:
      lcd.print(F(" Sounds toggle"));
      break;
    default:
      break;
  }
}

// About the game
void printAboutMenu() {
  displayImageInt64(matControl, hammer);
  lcd.setCursor(1, 0);
  lcd.print(F("    About     "));
}

void lowerMenuOption() {
  if (currentMenuOption > 1) {
    currentMenuOption--;
  }
}

void upperMenuOption() {
  currentMenuOption++;

  switch (currentMenu) {
    case 1:
      if (currentMenuOption > 1) {
        currentMenuOption = 1;
      }
    case 2:
      if (currentMenuOption > 2) {
        currentMenuOption = 2;
      }
  }
}

void leftMenu() {
  currentMenu--;
  currentMenuOption = 1;

  if (currentMenu < firstMenu) {
    currentMenu = lastMenu;
  }
}

void rightMenu() {
  currentMenu++;
  currentMenuOption = 1;

  if (currentMenu > lastMenu) {
    currentMenu = firstMenu;
  }
}

bool readMenuMovement() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

  if (xValue < minThreshold) {
    lowerMenuOption();

    lastMenuMovementTime = millis();

    return true;
  }

  if (xValue > maxThreshold) {
    upperMenuOption();

    lastMenuMovementTime = millis();

    return true;
  }

  if (yValue < minThreshold) {
    rightMenu();

    lastMenuMovementTime = millis();

    return true;
  }

  if (yValue > maxThreshold) {
    leftMenu();

    lastMenuMovementTime = millis();

    return true;
  }

  return false;
}
