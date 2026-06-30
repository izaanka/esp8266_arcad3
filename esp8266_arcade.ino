#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ESP8266 Pins
const int btnLeft = 14;   // D5
const int btnSelect = 12; // D6
const int btnRight = 13;  // D7

// --- Game Engine Base & Registry ---
class ArcadeGame {
public:
    virtual const char* getName() = 0;
    virtual void init() = 0;
    virtual void update() = 0;
};

#define MAX_GAMES 20
ArcadeGame* gamesRegistry[MAX_GAMES];
int registeredGamesCount = 0;

class GameRegistryEntry {
public:
    GameRegistryEntry(ArcadeGame* game) {
        if (registeredGamesCount < MAX_GAMES) {
            gamesRegistry[registeredGamesCount++] = game;
        }
    }
};

#define REGISTER_GAME(GameClass) \
    GameClass instance_##GameClass; \
    GameRegistryEntry entry_##GameClass(&instance_##GameClass);


// Forward Declaration for Games to return to menu
void returnToMenu();


// --- Game Includes ---
// To add a new game, simply drop its .h file in the folder and #include it here.
// It will automatically register itself to the main menu!
#include "LightsOut.h"
#include "Breakout.h"
#include "BlockJump.h"
#include "SimonSays.h"
#include "Minesweeper.h"
#include "SpaceShooter.h"
#include "TicTacToe.h"
#include "Tetris.h"
#include "FlappyBird.h"
#include "Pacman.h"
#include "ReactionTime.h"
#include "Stacker.h"
#include "GeometryDash.h"
#include "Pinball.h"
#include "Snake.h"
#include "DoodleJump.h"


// --- Main Program ---
int masterState = 0; // 0: Master Menu, 1: Playing Game
int masterMenuOption = 0;
ArcadeGame* currentGame = nullptr;

void returnToMenu() {
    masterState = 0;
    currentGame = nullptr;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); 

  pinMode(btnLeft, INPUT_PULLUP);
  pinMode(btnSelect, INPUT_PULLUP);
  pinMode(btnRight, INPUT_PULLUP);
  
  Wire.begin(5, 4);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  randomSeed(RANDOM_REG32); 
}

void loop() {
  // ==========================================
  // HOME SCREEN GLOBAL INTERRUPT (D5 Long Press)
  // ==========================================
  static unsigned long d5Timer = 0;
  if (digitalRead(btnLeft) == LOW) {
    if (d5Timer == 0) d5Timer = millis();
    else if (millis() - d5Timer > 1000) { 
      returnToMenu();
      return; 
    }
  } else {
    d5Timer = 0;
  }

  // ==========================================
  // MASTER MENU STATE
  // ==========================================
  if (masterState == 0) {
    if (digitalRead(btnLeft) == LOW) { 
        masterMenuOption = (masterMenuOption > 0) ? masterMenuOption - 1 : registeredGamesCount; 
        delay(150); 
    }
    if (digitalRead(btnRight) == LOW) { 
        masterMenuOption = (masterMenuOption < registeredGamesCount) ? masterMenuOption + 1 : 0; 
        delay(150); 
    }
    if (digitalRead(btnSelect) == LOW) {
        delay(200);
        if (masterMenuOption == registeredGamesCount) {
            masterState = 2; // Settings
        }
        else if (registeredGamesCount > 0) {
            currentGame = gamesRegistry[masterMenuOption];
            currentGame->init();
            masterState = 1;
        }
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(20, 0); display.print("- ARCADE MENU -");
    
    // Dynamic Scrolling Menu logic
    int startItem = masterMenuOption >= 4 ? masterMenuOption - 3 : 0;
    
    for (int i = 0; i < 4; i++) {
      int idx = startItem + i;
      if (idx > registeredGamesCount) break;
      display.setCursor(10, 15 + (i * 12));
      if (masterMenuOption == idx) display.print("> ");
      
      if (idx < registeredGamesCount) {
        display.print(idx + 1);
        display.print(". ");
        display.print(gamesRegistry[idx]->getName());
      } else {
        display.print(idx + 1);
        display.print(". Settings");
      }
    }
    
    display.display();
  }
  
  // ==========================================
  // PLAYING GAME STATE
  // ==========================================
  else if (masterState == 1) {
      if (currentGame != nullptr) {
          currentGame->update();
      }
  }

  // ==========================================
  // SETTINGS STATE
  // ==========================================
  else if (masterState == 2) {
      if (digitalRead(btnSelect) == LOW || digitalRead(btnLeft) == LOW || digitalRead(btnRight) == LOW) {
          delay(200);
          returnToMenu();
      }

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      
      display.setCursor(40, 0); 
      display.print("SETTINGS");
      
      display.setCursor(0, 15);
      display.print("Games Inst: "); 
      display.print(registeredGamesCount);
      
      uint32_t sketchSize = ESP.getSketchSize();
      uint32_t totalSpace = ESP.getFlashChipRealSize();
      
      display.setCursor(0, 27);
      display.print("Storage: ");
      display.print(sketchSize / 1024); display.print("/");
      display.print(totalSpace / 1024); display.print(" KB");
      
      display.setCursor(0, 43);
      display.print("creator :");
      display.setCursor(0, 53);
      display.print("github.com/izaanka");
      
      display.display();
  }
}
