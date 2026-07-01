#pragma once

class TrampolineGame : public ArcadeGame {
private:
    float py, pvy;
    float maxH;
    int state;
    bool lastS;
    bool boosted;
    int score;

public:
    const char* getCategory() override { return "Action"; }
    const char* getName() override { return "Trampoline"; }
    int getScore() override { return score; }

    void init() override {
        py = 20; pvy = 0; maxH = 20; state = 0; lastS = true; score = 0;
        boosted = false;
    }

    void update() override {
        if (state == 0) {
            bool cS = (digitalRead(btnSelect) == LOW);

            py += pvy;
            pvy += 0.15; // gravity

            if (py > 55) { // Hit trampoline
                float bouncePvy = -abs(pvy) * 0.8; // base loss
                if (boosted) {
                    bouncePvy -= 2.0; // gain height!
                    score++;
                }
                
                if (bouncePvy > -2.0) { state = 1; delay(500); } // lost all momentum
                else {
                    pvy = bouncePvy;
                    py = 55;
                }
                boosted = false;
            }

            // Input to boost
            if (cS && !lastS && !boosted) {
                // Must press exactly right before hitting (py > 45 && py < 55)
                if (py > 45 && py < 55 && pvy > 0) {
                    boosted = true;
                } else if (py < 45 && pvy > 0) {
                    // Pressed too early, ruin jump
                    pvy *= 0.5;
                }
            }
            lastS = cS;

            display.clearDisplay();
            
            // Trampoline
            display.drawLine(40, 58, 88, 58, WHITE);
            display.drawLine(40, 58, 35, 64, WHITE);
            display.drawLine(88, 58, 93, 64, WHITE);

            // Player
            display.fillRect(60, (int)py - 8, 8, 8, WHITE);

            // Boost effect
            if (boosted) display.drawCircle(64, (int)py - 4, 8, WHITE);

            display.setCursor(0, 0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(20, 20); display.print("MOMENTUM LOST!");
            display.setCursor(20, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(TrampolineGame)
