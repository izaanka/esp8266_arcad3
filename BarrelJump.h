#pragma once

class BarrelJumpGame : public ArcadeGame {
private:
    float py, pvy;
    bool jumping;
    int score;
    int state;
    bool lastS;

    struct Barrel { float x; };
    Barrel barrels[3];
    float speed;

public:
    const char* getCategory() override { return "Action"; }
    const char* getName() override { return "Barrel Jump"; }
    int getScore() override { return score; }

    void init() override {
        py = 50; pvy = 0; jumping = false; score = 0; state = 0;
        speed = 2.0; lastS = true;
        for(int i=0; i<3; i++) barrels[i].x = 128 + i*60;
    }

    void update() override {
        if (state == 0) {
            bool cS = (digitalRead(btnSelect) == LOW);

            if (cS && !lastS && !jumping) {
                jumping = true; pvy = -4.5;
            }
            lastS = cS;

            if (jumping) {
                py += pvy; pvy += 0.3;
                if (py >= 50) { py = 50; jumping = false; }
            }

            for(int i=0; i<3; i++) {
                barrels[i].x -= speed;
                if (barrels[i].x < -10) {
                    barrels[i].x = 128 + random(20, 60);
                    score++;
                    if (score % 5 == 0) speed += 0.2;
                }

                // Collision
                if (barrels[i].x > 10 && barrels[i].x < 22) {
                    if (py > 42) { state = 1; delay(500); }
                }
            }

            display.clearDisplay();
            
            // Ground
            display.drawLine(0, 52, 128, 52, WHITE);
            
            // Player
            display.fillRect(14, (int)py - 8, 8, 10, WHITE);
            
            // Barrels
            for(int i=0; i<3; i++) {
                if (barrels[i].x > -10 && barrels[i].x < 128) {
                    display.drawCircle((int)barrels[i].x, 48, 4, WHITE);
                    display.drawLine((int)barrels[i].x-2, 48, (int)barrels[i].x+2, 48, WHITE);
                }
            }

            display.setCursor(0, 0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("SMASHED!");
            display.setCursor(30, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(BarrelJumpGame)
