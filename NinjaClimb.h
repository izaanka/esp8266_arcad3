#pragma once

class NinjaClimbGame : public ArcadeGame {
private:
    float pY;
    bool onLeft;
    bool jumping;
    float pX;
    float jV;
    int score;
    int state;
    
    struct Spike { int y; bool isLeft; };
    Spike spikes[4];

public:
    const char* getCategory() override { return "Action"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Ninja Climb"; }

    void init() override {
        pY = 50; onLeft = true; jumping = false; pX = 10;
        score = 0; state = 0;
        for(int i=0; i<4; i++) {
            spikes[i].y = -i * 30;
            spikes[i].isLeft = random(2);
        }
    }

    void update() override {
        if (state == 0) {
            if (!jumping) {
                if (onLeft && digitalRead(btnRight) == LOW) { jumping = true; jV = 4.0; }
                else if (!onLeft && digitalRead(btnLeft) == LOW) { jumping = true; jV = -4.0; }
            } else {
                pX += jV;
                if (pX >= 110) { pX = 110; jumping = false; onLeft = false; score++; }
                if (pX <= 10) { pX = 10; jumping = false; onLeft = true; score++; }
            }

            for(int i=0; i<4; i++) {
                spikes[i].y += 2;
                if (spikes[i].y > 64) {
                    spikes[i].y = -30;
                    spikes[i].isLeft = random(2);
                    score++;
                }

                // Collision
                int sx = spikes[i].isLeft ? 5 : 115;
                if (spikes[i].y > pY - 8 && spikes[i].y < pY + 8) {
                    if ((spikes[i].isLeft && pX < 18) || (!spikes[i].isLeft && pX > 102)) {
                        state = 1; delay(500);
                    }
                }
            }

            display.clearDisplay();
            display.drawLine(5, 0, 5, 64, WHITE);
            display.drawLine(123, 0, 123, 64, WHITE);

            display.fillRect((int)pX, (int)pY - 4, 8, 8, WHITE);

            for(int i=0; i<4; i++) {
                if (spikes[i].isLeft) display.fillTriangle(5, spikes[i].y, 13, spikes[i].y+4, 5, spikes[i].y+8, WHITE);
                else display.fillTriangle(123, spikes[i].y, 115, spikes[i].y+4, 123, spikes[i].y+8, WHITE);
            }

            display.setCursor(60, 0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("SPLAT!");
            display.setCursor(35, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(NinjaClimbGame)
