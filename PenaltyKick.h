#pragma once

class PenaltyKickGame : public ArcadeGame {
private:
    int state; // 0=aim, 1=shoot, 2=result
    int score;
    int goalX; // 0=L, 1=C, 2=R
    int shotX; // 0=L, 1=C, 2=R
    float ballY;

public:
    const char* getCategory() override { return "Sports"; }
    const char* getName() override { return "Penalty Kick"; }
    int getScore() override { return score; }

    void init() override {
        score = 0; state = 0;
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);

            shotX = -1;
            if (cL) shotX = 0;
            if (cS) shotX = 1;
            if (cR) shotX = 2;

            if (shotX != -1) {
                // AI picks a direction to dive
                goalX = random(3);
                state = 1;
                ballY = 50;
                delay(200);
            }

            display.clearDisplay();
            display.setCursor(20, 40); display.print("L   C   R");
            
            // Goal
            display.drawRect(24, 10, 80, 20, WHITE);
            // Goalie
            display.fillRect(60, 20, 8, 10, WHITE);
            // Ball
            display.fillCircle(64, 50, 4, WHITE);
            
            display.setCursor(0, 0); display.print("Score: "); display.print(score);
            display.display();
            delay(15);
        } else if (state == 1) { // Shooting animation
            ballY -= 2.0;
            if (ballY <= 25) {
                state = 2; delay(500);
            }

            display.clearDisplay();
            display.drawRect(24, 10, 80, 20, WHITE);
            
            // Draw Goalie diving
            if (goalX == 0) display.fillRect(34, 20, 10, 8, WHITE);
            else if (goalX == 1) display.fillRect(60, 20, 8, 10, WHITE);
            else display.fillRect(84, 20, 10, 8, WHITE);

            // Draw Ball
            float bX = 64;
            if (shotX == 0) bX -= (50 - ballY);
            if (shotX == 2) bX += (50 - ballY);
            display.fillCircle((int)bX, (int)ballY, 4, WHITE);
            
            display.display();
            delay(15);
        } else if (state == 2) {
            display.clearDisplay();
            if (shotX == goalX) {
                display.setCursor(40, 20); display.print("SAVED!");
                display.setCursor(40, 35); display.print("Score: "); display.print(score);
                if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
                if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            } else {
                display.setCursor(40, 20); display.print("GOAL!");
                if (digitalRead(btnSelect) == LOW) { score++; state = 0; delay(200); }
            }
            display.display();
        }
    }
};

REGISTER_GAME(PenaltyKickGame)
