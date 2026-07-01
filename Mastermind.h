#pragma once

class MastermindGame : public ArcadeGame {
private:
    int secret[4];
    int currentGuess[4];
    int guessIdx = 0;
    int tries = 0;
    int state = 0;
    
    struct History { int guess[4]; int black; int white; };
    History history[10];
    
    bool lastSelect = true;

public:
    const char* getCategory() override { return "Puzzle"; }
    const char* getName() override { return "Mastermind"; }

    void init() override {
        for(int i=0; i<4; i++) secret[i] = random(1, 7); // Colors 1-6
        for(int i=0; i<4; i++) currentGuess[i] = 1;
        guessIdx = 0; tries = 0; state = 0; lastSelect = true;
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) {
                currentGuess[guessIdx]--;
                if (currentGuess[guessIdx] < 1) currentGuess[guessIdx] = 6;
                delay(150);
            }
            if (digitalRead(btnRight) == LOW) {
                currentGuess[guessIdx]++;
                if (currentGuess[guessIdx] > 6) currentGuess[guessIdx] = 1;
                delay(150);
            }
            
            bool currSelect = (digitalRead(btnSelect) == LOW);
            if (currSelect && !lastSelect) {
                guessIdx++;
                if (guessIdx >= 4) {
                    // Evaluate
                    int black = 0; int white = 0;
                    bool secUsed[4] = {false, false, false, false};
                    bool guessUsed[4] = {false, false, false, false};
                    
                    for(int i=0; i<4; i++) {
                        if (currentGuess[i] == secret[i]) {
                            black++; secUsed[i] = true; guessUsed[i] = true;
                        }
                    }
                    for(int i=0; i<4; i++) {
                        if (!guessUsed[i]) {
                            for(int j=0; j<4; j++) {
                                if (!secUsed[j] && currentGuess[i] == secret[j]) {
                                    white++; secUsed[j] = true; break;
                                }
                            }
                        }
                    }
                    
                    if (tries < 10) {
                        for(int i=0; i<4; i++) history[tries].guess[i] = currentGuess[i];
                        history[tries].black = black; history[tries].white = white;
                    }
                    
                    tries++;
                    if (black == 4) state = 1; // Win
                    else if (tries >= 8) state = 2; // Lose (max 8 tries displayed nicely)
                    
                    guessIdx = 0;
                    for(int i=0; i<4; i++) currentGuess[i] = 1;
                }
            }
            lastSelect = currSelect;
            
            display.clearDisplay();
            
            // Draw History
            for(int i=0; i<tries; i++) {
                int y = i * 7;
                for(int j=0; j<4; j++) {
                    display.setCursor(j*10 + 10, y); display.print(history[i].guess[j]);
                }
                display.setCursor(60, y); display.print("B:"); display.print(history[i].black);
                display.setCursor(85, y); display.print("W:"); display.print(history[i].white);
            }
            
            // Draw Current
            int cy = tries * 7;
            for(int j=0; j<4; j++) {
                if (j == guessIdx) display.setTextColor(BLACK, WHITE);
                display.setCursor(j*10 + 10, cy); 
                if (j <= guessIdx) display.print(currentGuess[j]);
                else display.print("-");
                display.setTextColor(WHITE, BLACK);
            }
            
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); 
            if (state == 1) display.print("CRACKED IT!"); else display.print("FAILED!");
            display.setCursor(30, 35);
            display.print("Code: ");
            for(int i=0; i<4; i++) display.print(secret[i]);
            display.setCursor(20, 50); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(MastermindGame)
