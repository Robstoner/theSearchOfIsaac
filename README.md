# The Search of Isaac - An Arduino game based on The Binding of Isaac

## Description

This game is made as a project for the Introduction to Robotics course at UB-FMI. It is based on the game The Binding of Isaac, on a 8x8 LED matrix and a 16x2 LCD display. The player has to find treasures while avoiding obstacles. There are multiple rooms, with multiple treasures, and the player has to find as many as they can before the time runs out. They also need to get back to the start before the time runs out and escape, so being greedy can be dangerous.

## Rules

The player starts in a room with a random number of treasures and walls. An RGB LED will light up in different colors to signify how close to a treasure you are, and you will have to dig it up from below you. If the spot you dig up is not a treasure, you lose one shovel. There will be multiple rooms per level, and you will have to find all the treasures and go back to the start to go to the next level. If you run out of shovels, you lose. If you run out of time, you lose. If you find all the treasures and go back to the start, you win.