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
      lcd.print(F(" Sounds "));
      if (soundsToggle) {
        lcd.print(F("off"));
      } else {
        lcd.print(F("on"));
      }

      break;
    default:
      break;
  }
}

// About the game
void printAboutMenu(byte option) {
  displayImageInt64(matControl, hammer);

  switch (option) {
    case 1:
      lcd.setCursor(1, 0);
      lcd.print(F("    About     "));
      lcd.setCursor(0, 1);
      lcd.print(F("TheSearchOfIsaac"));
      break;
    case 2:
      lcd.setCursor(1, 0);
      lcd.print(F("    Creator   "));
      lcd.setCursor(0, 1);
      lcd.print(F("    Robstoner   "));
      break;
    case 3:
      lcd.setCursor(1, 0);
      lcd.print(F("     Link     "));
      lcd.setCursor(0, 1);
      lcd.print(F("   t.ly/oiQ77   "));
    default:
      break;
  }
}

bool lowerMenuOption() {
  if (currentMenuOption > 1) {
    currentMenuOption--;
    return true;
  }

  return false;
}

bool upperMenuOption() {

  switch (currentMenu) {
    case 1:
      if (currentMenuOption < 1) {
        currentMenuOption++;
        return true;
      }
      break;
    case 2:
      if (currentMenuOption < 2) {
        currentMenuOption++;
        return true;
      }
      break;
    case 3:
      if (currentMenuOption < 3) {
        currentMenuOption++;
        return true;
      }
      break;
    case 4:
      if (currentMenuOption < 3) {
        currentMenuOption++;
        return true;
      }
      break;
    default:
      break;
  }

  return false;
}

bool leftMenu() {
  currentMenu--;
  currentMenuOption = 1;

  if (currentMenu < firstMenu) {
    currentMenu = lastMenu;
  }

  return true;
}

bool rightMenu() {
  currentMenu++;
  currentMenuOption = 1;

  if (currentMenu > lastMenu) {
    currentMenu = firstMenu;
  }

  return true;
}

bool readMenuMovement() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

  if (xValue < minThreshold) {
    lastMenuMovementTime = millis();

    return lowerMenuOption();
  }

  if (xValue > maxThreshold) {
    lastMenuMovementTime = millis();

    return upperMenuOption();
  }

  if (yValue < minThreshold) {
    lastMenuMovementTime = millis();

    return rightMenu();
  }

  if (yValue > maxThreshold) {
    lastMenuMovementTime = millis();

    return leftMenu();
  }

  return false;
}
