#pragma once

class DivingGame : public ArcadeGame {
private:
    float py, pvy;
    float angle;
    int state;
    int score;

public:
    const char* getCategory() override { return "Sports"; }
    const char* getName() override { return "Diving"; }
    int getScore() override { return score; }

    void init() override {
        py = 10; pvy = -2.0; angle = 0; state = 0; score = 0;
    }

    void loadDive() {
        py = 10; pvy = -2.0; angle = 0; state = 0;
    }

    void update() override {
        if (state == 0) { // Jumping
            py += pvy; pvy += 0.15; // gravity
            
            if (digitalRead(btnSelect) == LOW) {
                angle += 0.3; // Spin!
            }
            
            if (angle > TWO_PI) angle -= TWO_PI;

            if (py > 50) { // Hit water
                // Check if vertical (angle near 0 or PI)
                float normAngle = angle;
                if (normAngle > PI) normAngle -= PI;
                
                if (normAngle < 0.3 || normAngle > 2.8) {
                    score += 10; state = 1; delay(500); // Perfect
                } else if (normAngle < 0.6 || normAngle > 2.5) {
                    score += 5; state = 1; delay(500); // OK
                } else {
                    state = 2; delay(500); // Belly flop
                }
            }

            display.clearDisplay();
            
            // Diving board
            if (py < 20) display.drawLine(0, 15, 30, 15, WHITE);
            
            // Water
            display.drawLine(0, 55, 128, 55, WHITE);

            // Diver
            float dx = sin(angle) * 6;
            float dy = cos(angle) * 6;
            display.drawLine(64 - dx, py - dy, 64 + dx, py + dy, WHITE);
            // Head indicator
            display.fillCircle(64 - dx, py - dy, 2, WHITE);

            display.setCursor(0,0); display.print(score);
            display.display();
            delay(15);
        } else if (state == 1) { // Success
            display.clearDisplay();
            display.setCursor(40, 20); display.print("SPLASH!");
            if (digitalRead(btnSelect) == LOW) { loadDive(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        } else if (state == 2) { // Fail
            display.clearDisplay();
            display.setCursor(30, 20); display.print("BELLY FLOP!");
            display.setCursor(30, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(DivingGame)
