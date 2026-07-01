#pragma once

class JumpRopeGame : public ArcadeGame {
private:
    float ropeAngle;
    float speed;
    bool jumping;
    float pY, pVY;
    int score;
    int state;
    bool lastSelect;

public:
    const char* getCategory() override { return "Action"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Jump Rope"; }

    void init() override {
        ropeAngle = 0; speed = 0.05;
        jumping = false; pY = 50; pVY = 0;
        score = 0; state = 0; lastSelect = true;
    }

    void update() override {
        if (state == 0) {
            bool cS = (digitalRead(btnSelect) == LOW);
            if (cS && !lastSelect && !jumping) {
                jumping = true; pVY = -4.0;
            }
            lastSelect = cS;

            if (jumping) {
                pY += pVY; pVY += 0.4;
                if (pY >= 50) { pY = 50; jumping = false; }
            }

            ropeAngle += speed;
            if (ropeAngle > TWO_PI) { 
                ropeAngle -= TWO_PI; 
                score++; 
                speed += 0.005; 
            }

            // Rope bounds check (rope passes under player near angle PI/2)
            if (ropeAngle > 1.2 && ropeAngle < 1.9) {
                if (pY > 40) { // Player didn't jump high enough
                    state = 1; delay(500);
                }
            }

            display.clearDisplay();
            display.drawLine(0, 50, 128, 50, WHITE);

            // Player
            display.fillRect(60, (int)pY - 10, 8, 10, WHITE);

            // Rope (ellipse approximation)
            float rx = 64 + cos(ropeAngle) * 40;
            float ry = 30 + sin(ropeAngle) * 20;
            // Draw rope arc depending on front/back
            if (sin(ropeAngle) > 0) { // Front
                display.drawLine(24, 30, (int)rx, (int)ry, WHITE);
                display.drawLine(104, 30, (int)rx, (int)ry, WHITE);
            } else { // Back
                // Draw lighter or dashed, but we only have solid line
                display.drawLine(24, 30, (int)rx, (int)ry, WHITE);
                display.drawLine(104, 30, (int)rx, (int)ry, WHITE);
                // Draw player OVER the rope in back
                display.fillRect(60, (int)pY - 10, 8, 10, BLACK); // mask
                display.drawRect(60, (int)pY - 10, 8, 10, WHITE);
            }

            display.setCursor(0, 0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("TRIPPED!");
            display.setCursor(35, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(JumpRopeGame)
