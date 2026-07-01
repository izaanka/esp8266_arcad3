#pragma once

class SpaceDockGame : public ArcadeGame {
private:
    float angle, aVel;
    float dist;
    float dockAngle;
    int state;
    int score;

public:
    const char* getCategory() override { return "Action"; }
    const char* getName() override { return "Space Dock"; }
    int getScore() override { return score; }

    void init() override {
        angle = 0; aVel = 0; dist = 50; dockAngle = PI; state = 0; score = 0;
    }

    void loadLevel() {
        dist = 50; aVel = 0; dockAngle = random(0, 314) * 0.02; state = 0;
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) aVel -= 0.01;
            if (digitalRead(btnRight) == LOW) aVel += 0.01;
            if (digitalRead(btnSelect) == LOW) dist -= 0.5; // thrust
            
            angle += aVel;
            if (angle > TWO_PI) angle -= TWO_PI;
            if (angle < 0) angle += TWO_PI;

            dockAngle += 0.01 + (score * 0.005); // Station rotates
            if (dockAngle > TWO_PI) dockAngle -= TWO_PI;
            
            if (dist <= 12) { // Reached station surface
                float diff = abs(angle - dockAngle);
                if (diff > PI) diff = TWO_PI - diff;
                
                if (diff < 0.2 && abs(aVel) < 0.05) { // Aligned & slow
                    score++; loadLevel(); delay(500);
                } else {
                    state = 1; delay(500); // CRASH!
                }
            }

            display.clearDisplay();
            
            // Station (center)
            display.drawCircle(64, 32, 12, WHITE);
            // Dock port
            float px = 64 + cos(dockAngle)*12;
            float py = 32 + sin(dockAngle)*12;
            display.fillCircle((int)px, (int)py, 3, BLACK); // cut hole
            display.drawLine(64 + cos(dockAngle)*9, 32 + sin(dockAngle)*9, 64 + cos(dockAngle)*15, 32 + sin(dockAngle)*15, WHITE);

            // Ship
            float sx = 64 + cos(angle)*dist;
            float sy = 32 + sin(angle)*dist;
            display.fillTriangle(
                sx, sy, 
                sx - cos(angle-0.3)*6, sy - sin(angle-0.3)*6, 
                sx - cos(angle+0.3)*6, sy - sin(angle+0.3)*6, WHITE);

            display.setCursor(0,0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("CRASHED!");
            display.setCursor(35, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(SpaceDockGame)
