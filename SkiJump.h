#pragma once

class SkiJumpGame : public ArcadeGame {
private:
    float angle;
    float dist;
    float speed;
    float py;
    int state; // 0=ramp, 1=air, 2=result
    int score;

public:
    const char* getCategory() override { return "Sports"; }
    const char* getName() override { return "Ski Jump"; }
    int getScore() override { return score; }

    void init() override {
        speed = 1.0; dist = 0; angle = 0; py = 20; state = 0; score = 0;
    }

    void update() override {
        if (state == 0) { // On ramp
            bool cS = (digitalRead(btnSelect) == LOW);
            if (cS) speed += 0.2; // hold to build speed
            
            py += speed * 0.5; // moving down ramp
            
            if (py > 50) { 
                if (!cS) state = 1; // Good release
                else { state = 2; score = 0; delay(500); } // Late release, crash
            }

            display.clearDisplay();
            display.drawLine(10, 10, 80, 50, WHITE); // ramp
            display.fillRect(10 + (py-10)*1.75, (int)py, 6, 6, WHITE);
            display.setCursor(0, 0); display.print("Hold S, release at end");
            display.display();
            delay(15);
        } else if (state == 1) { // In air
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);

            if (cL) angle -= 0.05;
            if (cR) angle += 0.05;
            
            py += 0.5 + abs(angle); // fall faster if unaligned
            dist += speed;
            speed *= 0.99; // drag

            if (py > 60) {
                if (abs(angle) < 0.3) {
                    score = (int)dist; state = 2; delay(500);
                } else {
                    score = 0; state = 2; delay(500); // crash landing
                }
            }

            display.clearDisplay();
            display.drawLine(0, 62, 128, 62, WHITE); // ground
            
            // Draw Skier with angle
            int sx = 64; int sy = (int)py;
            int dx = cos(angle) * 8; int dy = sin(angle) * 8;
            display.drawLine(sx - dx, sy - dy, sx + dx, sy + dy, WHITE); // skis
            display.fillCircle(sx, sy-4, 2, WHITE); // head

            display.setCursor(0, 0); display.print("Dist: "); display.print((int)dist);
            display.display();
            delay(15);
        } else if (state == 2) {
            display.clearDisplay();
            display.setCursor(30, 20); 
            if (score > 0) display.print("LANDED!"); else display.print("CRASHED!");
            display.setCursor(30, 35); display.print("Dist: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(SkiJumpGame)
