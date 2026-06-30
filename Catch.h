#pragma once

class CatchGame : public ArcadeGame {
private:
    float pX = 54;
    int score = 0;
    int lives = 3;
    int state = 0;
    
    struct Apple { float x, y, speed; bool active; };
    Apple apples[3];

    void spawnApple() {
        for(int i=0; i<3; i++) {
            if (!apples[i].active) {
                apples[i].active = true;
                apples[i].x = random(5, 123);
                apples[i].y = -10;
                apples[i].speed = random(15, 30) / 10.0 + (score * 0.05);
                break;
            }
        }
    }

public:
    const char* getName() override { return "Apple Catch"; }

    void init() override {
        pX = 54; score = 0; lives = 3; state = 0;
        for(int i=0; i<3; i++) apples[i].active = false;
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) pX -= 3.0;
            if (digitalRead(btnRight) == LOW) pX += 3.0;
            
            if (pX < 0) pX = 0;
            if (pX > 108) pX = 108; // basket is 20 wide
            
            if (random(100) < 4) spawnApple();
            
            for(int i=0; i<3; i++) {
                if (apples[i].active) {
                    apples[i].y += apples[i].speed;
                    
                    if (apples[i].y > 54 && apples[i].y < 60) {
                        if (apples[i].x + 4 > pX && apples[i].x < pX + 20) {
                            // Caught!
                            apples[i].active = false;
                            score++;
                        }
                    } else if (apples[i].y > 64) {
                        // Missed
                        apples[i].active = false;
                        lives--;
                        if (lives <= 0) {
                            state = 1;
                            delay(500);
                        }
                    }
                }
            }
            
            display.clearDisplay();
            
            // Draw basket
            display.drawLine(pX, 56, pX+20, 56, WHITE);
            display.drawLine(pX, 56, pX+4, 62, WHITE);
            display.drawLine(pX+20, 56, pX+16, 62, WHITE);
            display.drawLine(pX+4, 62, pX+16, 62, WHITE);
            display.drawLine(pX+10, 56, pX+10, 62, WHITE); // detail
            
            // Draw apples
            for(int i=0; i<3; i++) {
                if (apples[i].active) {
                    display.fillCircle((int)apples[i].x, (int)apples[i].y, 3, WHITE);
                    display.drawLine((int)apples[i].x, (int)apples[i].y-3, (int)apples[i].x+2, (int)apples[i].y-5, WHITE); // stem
                }
            }
            
            // Draw UI
            display.setCursor(0, 0); display.print("S:"); display.print(score);
            display.setCursor(100, 0); display.print("L:"); display.print(lives);
            
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(20, 20); display.print("GAME OVER");
            display.setCursor(20, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(CatchGame)
