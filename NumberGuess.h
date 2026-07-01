#pragma once

class NumberGuessGame : public ArcadeGame {
private:
    int target;
    int guess;
    int state; // 0=guess, 1=high, 2=low, 3=win, 4=loss
    int tries;
    int maxTries;
    int score;
    bool lastS;

public:
    const char* getCategory() override { return "Puzzle"; }
    const char* getName() override { return "Number Guess"; }
    int getScore() override { return score; }

    void loadLevel() {
        target = random(0, 100);
        guess = 50;
        tries = 0;
        maxTries = 7; // log2(100) approx
        state = 0;
        lastS = true;
    }

    void init() override {
        score = 0; loadLevel();
    }

    void update() override {
        bool cS = (digitalRead(btnSelect) == LOW);

        if (state == 0 || state == 1 || state == 2) {
            if (digitalRead(btnLeft) == LOW) { guess--; if(guess<0) guess=99; delay(100); }
            if (digitalRead(btnRight) == LOW) { guess++; if(guess>99) guess=0; delay(100); }
            
            if (cS && !lastS) {
                tries++;
                if (guess == target) {
                    state = 3; score++; delay(1000); loadLevel();
                } else if (tries >= maxTries) {
                    state = 4; delay(500);
                } else if (guess > target) {
                    state = 1; // Too high
                } else {
                    state = 2; // Too low
                }
            }
        } else if (state == 4) {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("GAME OVER");
            display.setCursor(30, 35); display.print("Target: "); display.print(target);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
            return;
        }
        
        lastS = cS;

        display.clearDisplay();
        display.setCursor(10, 0); display.print("Guess: 0-99");
        
        display.setTextSize(2);
        display.setCursor(50, 25);
        if (guess < 10) display.print("0");
        display.print(guess);
        display.setTextSize(1);
        
        display.setCursor(10, 50);
        if (state == 1) display.print("TOO HIGH!");
        else if (state == 2) display.print("TOO LOW!");
        
        display.setCursor(90, 50); display.print(maxTries - tries); display.print(" left");

        display.setCursor(100, 0); display.print("S:"); display.print(score);
        display.display();
        delay(15);
    }
};

REGISTER_GAME(NumberGuessGame)
