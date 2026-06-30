# ESP8266 Arcade

A modular, multi-game arcade platform built for the ESP8266 microcontroller and a 128x64 I2C OLED display.

This project features a dynamic, object-oriented architecture where games are self-registering. Adding a new game is as simple as dropping in a header file and including it in the main sketch!

## Included Games
- **Lights Out**: A classic puzzle game where you try to turn off all the lights on grids of varying sizes.
- **Breakout**: A brick-breaking game using a paddle and a ball.
- **Block Jump**: An endless runner where you jump over procedurally generated obstacles.
- **Simon Says**: A memory game that tests your pattern recognition.
- **Minesweeper**: The classic PC game, complete with flagging functionality.
- **Space Shooter**: Defend Earth from descending aliens!
- **Tic-Tac-Toe**: Play against a friend or a Minimax-powered AI.

---

## Hardware Requirements

- **ESP8266** (NodeMCU / Wemos D1 Mini recommended)
- **128x64 OLED Display** (SSD1306 I2C)
- **3 Push Buttons** (Left, Action, Right)
- Breadboard and jumper wires

### Pinout Configuration

Connect the hardware to your ESP8266 as follows:

| Component | ESP8266 Pin | GPIO Pin | Notes |
| :--- | :--- | :--- | :--- |
| **OLED SDA** | D1 | GPIO 5 | I2C Data |
| **OLED SCL** | D2 | GPIO 4 | I2C Clock |
| **Left Button** | D5 | GPIO 14 | Connect to Ground (Uses Internal Pullup) |
| **Select / Action**| D6 | GPIO 12 | Connect to Ground (Uses Internal Pullup) |
| **Right Button** | D7 | GPIO 13 | Connect to Ground (Uses Internal Pullup) |

> **Note:** Long pressing the **Left Button (D5)** at any point during gameplay will trigger a global interrupt and return you to the main menu.

---

## Architecture & Adding Your Own Game

This platform uses a modular architecture. The core engine (`esp8266_arcade.ino`) manages the display, hardware inputs, and the main menu. Games are encapsulated in their own `.h` files and inherit from the `ArcadeGame` base class.

### How to Add a New Game

You don't need to touch the main menu rendering code or input state machines to add a game. 

1. Create a new `.h` file in the project folder (e.g. `MyNewGame.h`).
2. Define your class, inheriting from `ArcadeGame`. Implement the `getName()`, `init()`, and `update()` methods.
3. At the bottom of your file, add the `REGISTER_GAME(YourClassName)` macro.
4. Finally, open `esp8266_arcade.ino` and `#include "MyNewGame.h"` in the "Game Includes" section.

#### Code Template (`MyNewGame.h`)

```cpp
#pragma once

class MyNewGame : public ArcadeGame {
private:
  int state = 90; // Suggestion: Use state machines for your game flow
  // Your game variables here...

public:
  const char *getName() override { 
      return "My New Game"; // This is what appears in the Main Menu!
  }

  void init() override {
    // Code to run when the user selects your game from the menu.
    // Reset scores, player positions, and timers here.
  }

  void update() override {
    // This runs in a loop continuously while your game is active.
    
    // Example interaction:
    // if (digitalRead(btnSelect) == LOW) { ... }
    
    // Remember to call returnToMenu(); when the game is over and 
    // the user wants to go back!
  }
};

// This macro handles appending the game to the menu array dynamically!
REGISTER_GAME(MyNewGame)
```

## Dependencies
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit SSD1306 Library](https://github.com/adafruit/Adafruit_SSD1306)