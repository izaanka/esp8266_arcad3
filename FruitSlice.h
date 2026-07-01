#pragma once

class FruitSliceGame : public ArcadeGame {
private:
    struct Fruit { float x, y, vy; bool active; };
    Fruit fruits[3];
    int score;
    int lives;
    int state;
    bool lastL, lastS, lastR;

    void spawnFruit(int i) {
        fruits[i].active = true;
        fruits[i].x = 24 + random(3) * 40; // 3 lanes: 24, 64, 104
        fruits[i].y = 64;
        fruits[i].vy = -random(40, 60) * 0.1; // -4.0 to -6.0
    }

public:
    const char* getCategory() override { return "Action"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Fruit Slice"; }

    void init() override {
        score = 0; lives = 3; state = 0;
        lastL = true; lastS = true; lastR = true;
        for(int i=0; i<3; i++) fruits[i].active = false;
        spawnFruit(0);
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);

            // Spawn logic
            if (random(100) < 3) {
                for(int i=0; i<3; i++) {
                    if (!fruits[i].active) { spawnFruit(i); break; }
                }
            }

            for(int i=0; i<3; i++) {
                if (fruits[i].active) {
                    fruits[i].y += fruits[i].vy;
                    fruits[i].vy += 0.15; // gravity
                    
                    if (fruits[i].y > 64 && fruits[i].vy > 0) {
                        fruits[i].active = false;
                        lives--;
                        if (lives <= 0) { state = 1; delay(500); }
                    }

                    // Slice check (Crosshairs at Y=32)
                    if (fruits[i].y > 24 && fruits[i].y < 40) {
                        bool sliced = false;
                        if (cL && !lastL && fruits[i].x < 44) sliced = true;
                        if (cS && !lastS && fruits[i].x > 44 && fruits[i].x < 84) sliced = true;
                        if (cR && !lastR && fruits[i].x > 84) sliced = true;
                        
                        if (sliced) {
                            fruits[i].active = false;
                            score++;
                        }
                    }
                }
            }

            lastL = cL; lastS = cS; lastR = cR;

            display.clearDisplay();
            
            // Crosshairs
            display.drawLine(14, 32, 34, 32, WHITE); display.drawLine(24, 22, 24, 42, WHITE);
            display.drawLine(54, 32, 74, 32, WHITE); display.drawLine(64, 22, 64, 42, WHITE);
            display.drawLine(94, 32, 114, 32, WHITE); display.drawLine(104, 22, 104, 42, WHITE);

            // Fruits
            for(int i=0; i<3; i++) {
                if (fruits[i].active) {
                    display.drawCircle((int)fruits[i].x, (int)fruits[i].y, 4, WHITE);
                }
            }

            // HUD
            display.setCursor(0, 0); display.print("S:"); display.print(score);
            display.setCursor(100, 0); display.print("L:"); display.print(lives);
            
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("GAME OVER!");
            display.setCursor(30, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(FruitSliceGame)
