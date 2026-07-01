#pragma once

class AirHockeyGame : public ArcadeGame {
private:
    float bx, by, bvx, bvy;
    float px, py;
    float apx, apy;
    int score;
    int state;
    int pScore, aiScore;

    void resetPuck() {
        bx = 64; by = 32; bvx = random(-2, 3); bvy = 2.0;
        if (bvx == 0) bvx = 1.0;
        state = 0;
    }

public:
    const char* getCategory() override { return "Sports"; }
    const char* getName() override { return "Air Hockey"; }
    int getScore() override { return score; }

    void init() override {
        px = 64; py = 55;
        apx = 64; apy = 10;
        pScore = 0; aiScore = 0; score = 0;
        resetPuck();
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) px -= 2.0;
            if (digitalRead(btnRight) == LOW) px += 2.0;
            if (px < 15) px = 15;
            if (px > 113) px = 113;

            // AI logic
            if (bx < apx - 4) apx -= 1.5;
            if (bx > apx + 4) apx += 1.5;
            if (apx < 15) apx = 15;
            if (apx > 113) apx = 113;

            // Puck physics
            bx += bvx; by += bvy;

            // Walls
            if (bx < 5) { bx = 5; bvx = -bvx; }
            if (bx > 123) { bx = 123; bvx = -bvx; }

            // Collision with Player Paddle
            float dx = bx - px; float dy = by - py;
            if (dx*dx + dy*dy < 64) { 
                bvy = -abs(bvy) - 0.2; 
                bvx = dx * 0.5;
            }

            // Collision with AI Paddle
            dx = bx - apx; dy = by - apy;
            if (dx*dx + dy*dy < 64) { 
                bvy = abs(bvy) + 0.2; 
                bvx = dx * 0.5;
            }

            // Goal detection
            if (by < 0) { // Player scored
                pScore++; score += 10;
                if (pScore >= 3) state = 1;
                else resetPuck();
            }
            if (by > 64) { // AI scored
                aiScore++;
                if (aiScore >= 3) state = 2;
                else resetPuck();
            }

            // Display
            display.clearDisplay();
            // Field
            display.drawRect(0, 0, 128, 64, WHITE);
            display.drawLine(0, 32, 128, 32, WHITE);
            // Goals
            display.drawLine(40, 0, 88, 0, BLACK);
            display.drawLine(40, 63, 88, 63, BLACK);
            // Puck
            display.fillCircle((int)bx, (int)by, 3, WHITE);
            // Paddles
            display.drawCircle((int)px, (int)py, 6, WHITE);
            display.drawCircle((int)apx, (int)apy, 6, WHITE);
            
            // Score
            display.setCursor(2, 28); display.print(aiScore);
            display.setCursor(2, 34); display.print(pScore);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); 
            if (state == 1) display.print("YOU WON!");
            else display.print("AI WON!");
            display.setCursor(30, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(AirHockeyGame)
