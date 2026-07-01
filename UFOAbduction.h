#pragma once

class UFOAbductionGame : public ArcadeGame {
private:
    float ux, uy;
    float objX[3];
    int objType[3]; // 0=cow, 1=spiked tractor
    float scrollSpeed;
    bool beaming;
    int score;
    int state;

public:
    const char* getCategory() override { return "Action"; }
    const char* getName() override { return "UFO Abduct"; }
    int getScore() override { return score; }

    void init() override {
        ux = 64; uy = 15; score = 0; state = 0; scrollSpeed = 1.0;
        for(int i=0; i<3; i++) {
            objX[i] = 128 + i*60;
            objType[i] = (random(100) < 70) ? 0 : 1;
        }
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);
            beaming = (digitalRead(btnSelect) == LOW);

            if (cL) ux -= 2.0;
            if (cR) ux += 2.0;

            for(int i=0; i<3; i++) {
                objX[i] -= scrollSpeed;
                if (objX[i] < -10) {
                    objX[i] += 180;
                    objType[i] = (random(100) < 70) ? 0 : 1;
                    if (score % 5 == 0) scrollSpeed += 0.1;
                }

                // Abduction check
                if (beaming && abs(ux - objX[i]) < 12) {
                    if (objType[i] == 0) {
                        score++; objX[i] = -20; // abduct cow
                    } else if (objType[i] == 1) {
                        state = 1; delay(500); // abduct spike
                    }
                }
            }

            display.clearDisplay();
            
            // UFO
            display.fillCircle((int)ux, (int)uy, 6, WHITE);
            display.drawLine((int)ux-10, (int)uy+2, (int)ux+10, (int)uy+2, WHITE);
            
            // Beam
            if (beaming) {
                display.drawLine((int)ux-4, (int)uy+6, (int)ux-8, 54, WHITE);
                display.drawLine((int)ux+4, (int)uy+6, (int)ux+8, 54, WHITE);
            }

            // Ground
            display.drawLine(0, 54, 128, 54, WHITE);
            
            // Objects
            for(int i=0; i<3; i++) {
                if (objX[i] > -10 && objX[i] < 128) {
                    if (objType[i] == 0) {
                        display.fillRect((int)objX[i]-4, 50, 8, 4, WHITE); // cow
                    } else {
                        display.fillTriangle((int)objX[i]-5, 54, (int)objX[i]+5, 54, (int)objX[i], 46, WHITE); // spike
                    }
                }
            }

            display.setCursor(0, 0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(20, 20); display.print("SPKIE BEAMED!");
            display.setCursor(20, 35); display.print("Cows: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(UFOAbductionGame)
