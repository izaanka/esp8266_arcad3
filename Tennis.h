#pragma once

class TennisGame : public ArcadeGame {
private:
    float bx, by, bvx, bvy;
    int state;
    int score;
    bool lastS;

public:
    const char* getCategory() override { return "Sports"; }
    const char* getName() override { return "Tennis"; }
    int getScore() override { return score; }

    void init() override {
        bx = 10; by = 20; bvx = 2.0; bvy = -1.0;
        state = 0; score = 0; lastS = true;
    }

    void update() override {
        if (state == 0) {
            bool cS = (digitalRead(btnSelect) == LOW);

            bx += bvx; by += bvy;
            bvy += 0.1; // gravity

            if (by > 60) {
                by = 60; bvy = -bvy * 0.8; // bounce
            }

            // Hit net
            if (bx > 60 && bx < 68 && by > 40) {
                bvx = -bvx * 0.5; // fall
            }

            // Out of bounds / win loss
            if (bx > 128) {
                // AI didn't hit it? Actually AI hits it automatically if it crosses
                bvx = -random(20, 30) * 0.1;
                bvy = -random(10, 30) * 0.1;
                bx = 125;
            }
            if (bx < 0) {
                state = 1; delay(500); // you missed
            }

            // Player hit
            if (cS && !lastS) {
                if (bx < 30 && bx > 5 && by < 55) {
                    bvx = random(20, 30) * 0.1;
                    bvy = -random(20, 40) * 0.1;
                    score++;
                }
            }

            lastS = cS;

            display.clearDisplay();
            
            // Court
            display.drawLine(0, 60, 128, 60, WHITE);
            // Net
            display.drawLine(64, 60, 64, 45, WHITE);
            
            // Player
            display.fillRect(4, 45, 4, 15, WHITE);
            // AI
            display.fillRect(120, 45, 4, 15, WHITE);
            
            // Ball
            display.fillCircle((int)bx, (int)by, 2, WHITE);

            display.setCursor(0, 0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("OUT!");
            display.setCursor(30, 35); display.print("Rallies: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(TennisGame)
