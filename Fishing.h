#pragma once

class FishingGame : public ArcadeGame {
private:
    int state; // 0=waiting, 1=bite, 2=reeling, 3=win/loss
    int waitTimer;
    float tension;
    float fishPos;
    int reelProgress;
    int score;

public:
    const char* getCategory() override { return "Sports"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Fishing"; }

    void init() override {
        state = 0; waitTimer = random(50, 150);
        score = 0;
    }

    void update() override {
        bool cS = (digitalRead(btnSelect) == LOW);

        if (state == 0) {
            waitTimer--;
            if (waitTimer <= 0) { state = 1; waitTimer = 30; } // 30 frames to react
        } else if (state == 1) {
            waitTimer--;
            if (cS) { state = 2; tension = 50; fishPos = 50; reelProgress = 0; delay(200); }
            else if (waitTimer <= 0) { state = 3; score = -1; delay(500); } // missed
        } else if (state == 2) {
            fishPos += random(-4, 5); // fish struggles
            if (fishPos < 10) fishPos = 10;
            if (fishPos > 90) fishPos = 90;

            if (cS) tension += 3.0; else tension -= 2.0;

            if (tension < 0) { state = 3; score = -2; delay(500); } // got away
            if (tension > 100) { state = 3; score = -3; delay(500); } // line broke

            if (abs(tension - fishPos) < 15) reelProgress++;
            else reelProgress -= 1;
            if (reelProgress < 0) reelProgress = 0;

            if (reelProgress > 100) { state = 3; score = 1; delay(500); }
        }

        display.clearDisplay();

        if (state == 0) {
            display.setCursor(30, 30); display.print("Waiting...");
            display.drawLine(64, 0, 64, 40, WHITE);
            display.drawCircle(64, 42, 3, WHITE); // bobber
        } else if (state == 1) {
            display.setCursor(40, 30); display.print("BITE!");
            display.drawLine(64, 0, 64, 48, WHITE);
            display.fillCircle(64, 50, 4, WHITE);
        } else if (state == 2) {
            // Tension bar
            display.drawRect(10, 10, 108, 10, WHITE);
            display.fillRect(10 + fishPos - 15, 10, 30, 10, WHITE); // target zone
            display.drawLine(10 + tension, 5, 10 + tension, 25, WHITE); // tension line
            // Reel progress
            display.drawRect(10, 30, 108, 5, WHITE);
            display.fillRect(10, 30, reelProgress, 5, WHITE);
            display.setCursor(40, 45); display.print("Reeling...");
        } else if (state == 3) {
            display.setCursor(20, 20); 
            if (score == 1) display.print("CAUGHT FISH!");
            else if (score == -1) display.print("MISSED IT!");
            else if (score == -2) display.print("GOT AWAY!");
            else if (score == -3) display.print("LINE BROKE!");
            
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
        }

        display.display();
        delay(15);
    }
};

REGISTER_GAME(FishingGame)
