# The Search of Isaac - An Arduino game based on The Binding of Isaac

## Description

This game is made as a project for the Introduction to Robotics course at UB-FMI. It is based on the game The Binding of Isaac, on a 8x8 LED matrix and a 16x2 LCD display. The player has to find treasures while avoiding obstacles. There are multiple rooms, with multiple treasures, and the player has to find as many as they can before the time runs out, or all of them for a bonus.

## Rules

You have a number of shovels you can break until you lose. You break a shovel if you try to use it on a spot without a treasure.

The LED will indicate how far away you are from a treasure.
- RED - 2+ blocks away
- BLUE - 1 block away
- GREEN - on the same block

You will have a limited time to find all the treasures. If you do, you will get a bonus. If you don't, you will lose with only the score you got so far.

Not all levels are completeable. There may be treasures behind walls you cannot cross. If you find yourself in such a situation, you will be unable to get the final bonus. You can still try to get as many treasures as you can.

## Controls

You will use the joystick to move, and press down on the joystick to use a shovel.

The joystick will also be used to navigate the menu on the 16x2 LCD display.

## Hardware description

The components used are:
- Arduino Uno
- 8x8 LED matrix
- MAX7219 driver
- 16x2 LCD display
- Joystick
- Buzzer
- RGB LED
- 2 capacitors
- Resistors as needed
- Wires as needed

## Software description

The game is written in the folder [`theSearchOfIsaac`](theSearchOfIsaac). It is split into multiple files:
- [`theSearchOfIsaac.ino`](theSearchOfIsaac/theSearchOfIsaac.ino) - the main file, which contains all the main code for the game
- [`Pins.h`](theSearchOfIsaac/Pins.h) - contains all the pins used in the game
- [`Menu.h`](theSearchOfIsaac/Menu.h) - contains the code for the menu
- [`MatrixDrawings.h`](theSearchOfIsaac/MatrixDrawings) - contains the code for drawing on the LED matrix

The libraries needed to compile the code are `LedControl` and `LiquidCrystal`.