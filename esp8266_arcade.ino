#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ESP8266 Pins
const int btnLeft = 12;   //
const int btnSelect = 13; //
const int btnRight = 14;  //

// --- Game Engine Base & Registry ---
class ArcadeGame {
public:
    virtual const char* getName() = 0;
    virtual void init() = 0;
    virtual void update() = 0;
    virtual int getScore() { return 0; }
    virtual const char* getCategory() { return "Other Games"; }
};

#define MAX_GAMES 100
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
#include "Asteroids.h"
#include "Frogger.h"
#include "Pong.h"
#include "LunarLander.h"
#include "Racing.h"
#include "WhackAMole.h"
#include "Catch.h"
#include "MissileCommand.h"
#include "Memory.h"
#include "Hangman.h"
#include "Artillery.h"
#include "Rhythm.h"
#include "FallDown.h"
#include "Tron.h"
#include "Submarine.h"
#include "MathQuiz.h"
#include "Mastermind.h"
#include "Game2048.h"
#include "Sokoban.h"
#include "Connect4.h"
#include "Blackjack.h"
#include "Slots.h"
#include "Bowling.h"
#include "TRex.h"
#include "Helicopter.h"
#include "Invaders.h"
#include "DuckHunt.h"
#include "Parachute.h"
#include "RiverRaid.h"
#include "Rover.h"
#include "Skiing.h"
#include "BopIt.h"
#include "Timber.h"
#include "Maze.h"
#include "Darts.h"
#include "NinjaClimb.h"
#include "Fishing.h"
#include "LaserReflect.h"
#include "LockPicker.h"
#include "BalancingAct.h"
#include "Golf.h"
#include "Minecart.h"
#include "JumpRope.h"
#include "Hurdles.h"
#include "BalloonPop.h"
#include "Sumo.h"
#include "IceSkipping.h"
#include "FruitSlice.h"
#include "GoldMiner.h"
#include "CraneGame.h"
#include "DefuseBomb.h"
#include "WireLoop.h"
#include "JavelinThrow.h"
#include "SkateTrick.h"
#include "AirHockey.h"
#include "PenaltyKick.h"
#include "PizzaDelivery.h"
#include "RopeSwing.h"
#include "ColorMatch.h"
#include "RockPaperScissors.h"
#include "WhackAThief.h"
#include "BarrelJump.h"
#include "Tennis.h"
#include "BlobVolley.h"
#include "Karate.h"
#include "Plumber.h"
#include "NumberGuess.h"
#include "ZombieSurvival.h"
#include "Diving.h"
#include "Trampoline.h"
#include "SkiJump.h"
#include "TargetShooter.h"
#include "SpaceDock.h"
#include "HeliRescue.h"
#include "HillClimb.h"
#include "UFOAbduction.h"
#include "Miner.h"
#include "Bomberman.h"
#include "RadarSearch.h"
#include "BurgerChef.h"
#include "Juggling.h"
#include "TrafficCop.h"
#include "BungeeJump.h"
#include "WordScramble.h"
#include "MorseTranslator.h"
// --- Main Program ---
int masterState = 0; // 0: Master Menu, 1: Playing Game, 2: Settings, 3: Highscores, 4: Category Menu
int masterMenuOption = 0;
ArcadeGame* currentGame = nullptr;

bool enableHighScores = false;
int highscores[MAX_GAMES] = {0};

const char* categories[] = {
    "Games"
};
const int NUM_CATEGORIES = 1;

int currentCategoryGames[MAX_GAMES];
int numCategoryGames = 0;
int currentGameIndex = -1;

void returnToMenu() {
    if (masterState == 1) { // Playing a game
        if (enableHighScores && currentGame != nullptr && currentGameIndex != -1) {
            int s = currentGame->getScore();
            if (s > highscores[currentGameIndex]) {
                highscores[currentGameIndex] = s;
                EEPROM.put(1 + currentGameIndex * sizeof(int), s);
                EEPROM.commit();
            }
        }
        masterState = 4; // Go back to sub-menu
    } else {
        masterState = 0; // Go back to root menu
    }
    currentGame = nullptr;
    currentGameIndex = -1;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); 

  pinMode(btnLeft, INPUT_PULLUP);
  pinMode(btnSelect, INPUT_PULLUP);
  pinMode(btnRight, INPUT_PULLUP);
  
  Wire.begin(5, 4);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextWrap(false); // Fix text overflowing to next line
  randomSeed(RANDOM_REG32); 

  EEPROM.begin(512);
  uint8_t initByte = EEPROM.read(0);
  if (initByte == 1) enableHighScores = true;
  else enableHighScores = false;
  
  for (int i = 0; i < MAX_GAMES; i++) {
      EEPROM.get(1 + i * sizeof(int), highscores[i]);
      if (highscores[i] < 0 || highscores[i] > 999999) highscores[i] = 0; // Sanity check
  }
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
    int totalItems = NUM_CATEGORIES + 4;
    int morseIdx = NUM_CATEGORIES;
    int hsIdx = NUM_CATEGORIES + 1;
    int setIdx = NUM_CATEGORIES + 2;
    int credIdx = NUM_CATEGORIES + 3;

    if (digitalRead(btnLeft) == LOW) { 
        masterMenuOption = (masterMenuOption > 0) ? masterMenuOption - 1 : totalItems - 1; 
        delay(150); 
    }
    if (digitalRead(btnRight) == LOW) { 
        masterMenuOption = (masterMenuOption < totalItems - 1) ? masterMenuOption + 1 : 0; 
        delay(150); 
    }
    if (digitalRead(btnSelect) == LOW) {
        delay(200);
        if (masterMenuOption == credIdx) {
            masterState = 5; // Credits
        } else if (masterMenuOption == setIdx) {
            masterState = 2; // Settings
        } else if (masterMenuOption == hsIdx) {
            masterState = 3; // Highscores
            // Populate and sort all games for Highscores!
            numCategoryGames = 0;
            for(int i=0; i<registeredGamesCount; i++) {
                if (strcmp(gamesRegistry[i]->getName(), "Morse Translator") != 0) {
                    currentCategoryGames[numCategoryGames++] = i;
                }
            }
            for(int i=0; i<numCategoryGames-1; i++) {
                for(int j=i+1; j<numCategoryGames; j++) {
                    if (strcmp(gamesRegistry[currentCategoryGames[i]]->getName(), gamesRegistry[currentCategoryGames[j]]->getName()) > 0) {
                        int t = currentCategoryGames[i];
                        currentCategoryGames[i] = currentCategoryGames[j];
                        currentCategoryGames[j] = t;
                    }
                }
            }
        } else if (masterMenuOption == morseIdx) {
            for(int i=0; i<registeredGamesCount; i++) {
                if (strcmp(gamesRegistry[i]->getName(), "Morse Translator") == 0) {
                    currentGameIndex = i;
                    currentGame = gamesRegistry[i];
                    currentGame->init();
                    masterState = 1;
                    break;
                }
            }
        } else {
            // Load Sub-menu
            masterState = 4;
            numCategoryGames = 0;
            for(int i=0; i<registeredGamesCount; i++) {
                if (strcmp(gamesRegistry[i]->getName(), "Morse Translator") != 0) {
                    currentCategoryGames[numCategoryGames++] = i;
                }
            }
            // Sort
            for(int i=0; i<numCategoryGames-1; i++) {
                for(int j=i+1; j<numCategoryGames; j++) {
                    if (strcmp(gamesRegistry[currentCategoryGames[i]]->getName(), gamesRegistry[currentCategoryGames[j]]->getName()) > 0) {
                        int t = currentCategoryGames[i];
                        currentCategoryGames[i] = currentCategoryGames[j];
                        currentCategoryGames[j] = t;
                    }
                }
            }
            masterMenuOption = 0; // reset cursor for sub-menu
        }
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(20, 0); display.print("- ARCADE MENU -");
    
    int startItem = masterMenuOption >= 4 ? masterMenuOption - 3 : 0;
    
    for (int i = 0; i < 4; i++) {
      int idx = startItem + i;
      if (idx >= totalItems) break;
      display.setCursor(10, 15 + (i * 12));
      if (masterMenuOption == idx) display.print("> ");
      
      if (idx < NUM_CATEGORIES) {
        display.print("[ "); display.print(categories[idx]); display.print(" ]");
      } else if (idx == morseIdx) {
        display.print("[ Morse Translator ]");
      } else if (idx == hsIdx) {
        display.print("- Highscores");
      } else if (idx == setIdx) {
        display.print("- Settings");
      } else {
        display.print("- Credits");
      }
    }
    
    display.display();
  }
  // ==========================================
  // CATEGORY SUB-MENU STATE
  // ==========================================
  else if (masterState == 4) {
    if (digitalRead(btnLeft) == LOW) { 
        masterMenuOption = (masterMenuOption > 0) ? masterMenuOption - 1 : numCategoryGames - 1; 
        delay(150); 
    }
    if (digitalRead(btnRight) == LOW) { 
        masterMenuOption = (masterMenuOption < numCategoryGames - 1) ? masterMenuOption + 1 : 0; 
        delay(150); 
    }
    if (digitalRead(btnSelect) == LOW) {
        delay(200);
        if (numCategoryGames > 0) {
            currentGameIndex = currentCategoryGames[masterMenuOption];
            currentGame = gamesRegistry[currentGameIndex];
            currentGame->init();
            masterState = 1;
        }
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10, 0); display.print("- SELECT GAME -");
    
    int startItem = masterMenuOption >= 4 ? masterMenuOption - 3 : 0;
    
    for (int i = 0; i < 4; i++) {
      int idx = startItem + i;
      if (idx >= numCategoryGames) break;
      display.setCursor(10, 15 + (i * 12));
      if (masterMenuOption == idx) display.print("> ");
      
      int realIdx = currentCategoryGames[idx];
      display.print(gamesRegistry[realIdx]->getName());
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
      if (digitalRead(btnLeft) == LOW || digitalRead(btnRight) == LOW) {
          enableHighScores = !enableHighScores;
          EEPROM.write(0, enableHighScores ? 1 : 0);
          EEPROM.commit();
          delay(200);
      }
      if (digitalRead(btnSelect) == LOW) { delay(200); returnToMenu(); }

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      
      display.setCursor(30, 0); display.print("- SETTINGS -");
      
      display.setCursor(0, 15);
      display.print("< Highscores: ");
      display.print(enableHighScores ? "ON" : "OFF");
      display.print(" >");
      
      display.setCursor(0, 30);
      display.print("WARN: Saving scores");
      display.setCursor(0, 40);
      display.print("reduces flash memory");
      display.setCursor(0, 50);
      display.print("lifespan over time.");
      
      display.display();
  }
  // ==========================================
  // HIGHSCORES STATE
  // ==========================================
  else if (masterState == 3) {
      static int scroll = 0;
      if (digitalRead(btnLeft) == LOW) { scroll = (scroll > 0) ? scroll - 1 : registeredGamesCount - 1; delay(150); }
      if (digitalRead(btnRight) == LOW) { scroll = (scroll < registeredGamesCount - 1) ? scroll + 1 : 0; delay(150); }
      if (digitalRead(btnSelect) == LOW) { delay(200); returnToMenu(); }
      
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(20, 0); display.print("- HIGHSCORES -");
      
      if (!enableHighScores) {
          display.setCursor(0, 20); display.print("Highscores are OFF.");
          display.setCursor(0, 30); display.print("Enable in Settings");
      } else {
          int start = scroll;
          for(int i=0; i<4; i++) {
              int idx = start + i;
              if (idx >= registeredGamesCount) break;
              int realIdx = currentCategoryGames[idx]; // Uses array sorted in masterState 0
              display.setCursor(0, 15 + (i*12));
              display.print(gamesRegistry[realIdx]->getName());
              display.setCursor(100, 15 + (i*12));
              display.print(highscores[realIdx]);
          }
      }
      display.display();
  }
  // ==========================================
  // CREDITS STATE
  // ==========================================
  else if (masterState == 5) {
      if (digitalRead(btnSelect) == LOW || digitalRead(btnLeft) == LOW || digitalRead(btnRight) == LOW) { delay(200); returnToMenu(); }
      
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(32, 0); display.print("- CREDITS -");
      display.setCursor(0, 10); display.print("Licence: MIT Licence");
      display.setCursor(0, 19); display.print("Developer: Izaan");
      display.setCursor(0, 28); display.print("Kaskar");
      display.setCursor(0, 37); display.print("Project Repositary:");
      display.setCursor(0, 46); display.print("github.com/izaanka/");
      display.setCursor(0, 55); display.print("esp8266_arcad3");
      display.display();
  }
}
