#pragma once

class HeliRescueGame : public ArcadeGame {
private:
    float hx;
    float ropeLen;
    float boatX;
    int state;
    int score;

public:
    const char* getCategory() override { return "Action"; }
    const char* getName() override { return "Heli Rescue"; }
    int getScore() override { return score; }

    void init() override {
        hx = 64; ropeLen = 0; boatX = 64; state = 0; score = 0;
    }

    void loadLevel() {
        ropeLen = 0; boatX = random(20, 108); state = 0;
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);

            if (cL) hx -= 1.5;
            if (cR) hx += 1.5;
            
            if (cS) {
                ropeLen += 1.5;
                if (ropeLen > 40) { // reached water level
                    if (abs(hx - boatX) < 10) { // Aligned with boat
                        score++; loadLevel(); delay(500);
                    } else { // Missed boat
                        state = 1; delay(500);
                    }
                }
            } else {
                ropeLen -= 2.0; if(ropeLen<0) ropeLen=0;
            }

            boatX += sin(millis() / 400.0) * 1.5; // boat drifts on waves

            display.clearDisplay();
            
            // Heli
            display.fillRect((int)hx - 8, 10, 16, 6, WHITE);
            display.drawLine((int)hx - 12, 8, (int)hx + 12, 8, WHITE); // blades
            
            // Rope
            display.drawLine((int)hx, 16, (int)hx, 16 + (int)ropeLen, WHITE);
            
            // Water
            display.drawLine(0, 56, 128, 56, WHITE);
            
            // Boat
            display.fillRect((int)boatX - 10, 52, 20, 6, WHITE);

            display.setCursor(0, 0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("DROWNED!");
            display.setCursor(30, 35); display.print("Saved: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(HeliRescueGame)
