# ESP8266 OLED Arcade Platform

Welcome to the ultimate custom arcade platform running on a single ESP8266!

## Features
- **Massive Library**: Contains exactly **100 unique, hand-crafted games** spanning logic puzzles, physics simulations, racing, timing, and action!
- **Auto-Registry Engine**: Modular `.h` architecture. Just drop a game file in the folder, `#include` it, and the engine automatically injects it into the main menu without modifying `setup()` or `loop()`.
- **Categorized Folders**: 100 games are cleanly organized into 5 folders (`Arcade`, `Puzzle`, `Action`, `Sports`, `Other Games`) and sorted alphabetically for fast navigation.
- **EEPROM Highscores**: Automatically tracks your top scores for every single game. Saves only when you beat a highscore to drastically reduce flash memory wear.
- **Dynamic Menus**: Features a scrolling master list, a dedicated highscores screen, and an interactive settings toggler.
- **Hardware**: Uses an ESP8266, a 128x64 I2C OLED display (SSD1306), and just 3 tactile buttons (Left, Select, Right).

## The 100 Game Milestone
This project has reached its architectural limit of 100 distinct games. Highlights include:
- **Classics**: Tetris, Pong, Pacman, Space Invaders, Asteroids, Snake, Flappy Bird, Geometry Dash
- **Logic & Puzzles**: 2048, Minesweeper, Mastermind, Hangman, LightsOut, Plumber, Bomberman, Word Scramble
- **Physics**: Air Hockey, Rope Swing (Tarzan), Hill Climb, Bungee Jump, Ski Jump, Blob Volley
- **Action**: Zombie Survival, Karate, Heli Rescue, Target Shooter, Space Dock, UFO Abduction
- **Sports**: Tennis, Bowling, Golf, Penalty Kick, Diving, Javelin Throw, Sumo

## Setup
1. Wire up the SSD1306 OLED via I2C (SDA to D1, SCL to D2).
2. Wire up 3 tactile buttons to D5 (Left), D6 (Select), and D7 (Right). Connect the other legs to Ground.
3. Open `esp8266_arcade.ino` in the Arduino IDE.
4. Upload to your ESP8266.

*Tip: Enable Highscores in the Settings menu before playing!*

*readme.md was AI generated, there might be mistakes, contact owner incase of any*
