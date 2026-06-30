#pragma once

class FroggerGame : public ArcadeGame {
private:
    float pX, pY;
    int score = 0;
    int state = 0; // 0=play, 1=dead
    bool lastSelect = true;
    
    struct Obj { float x, y, speed, w; bool isLog; };
    Obj rows[12]; // 3 cars, 3 logs per screen
    
    void resetLevel() {
        pX = 60; pY = 56;
        for (int i=0; i<3; i++) {
            rows[i].x = random(0, 128); rows[i].y = 44 - (i*10);
            rows[i].speed = (random(2)==0?1:-1) * (random(10, 25)/10.0 + score*0.1);
            rows[i].w = 12; rows[i].isLog = false;
        }
        for (int i=3; i<6; i++) {
            rows[i].x = random(0, 128); rows[i].y = 14 - ((i-3)*10);
            rows[i].speed = (random(2)==0?1:-1) * (random(8, 20)/10.0 + score*0.05);
            rows[i].w = random(16, 28); rows[i].isLog = true;
        }
    }

public:
    const char* getName() override { return "Frogger"; }

    void init() override {
        score = 0; state = 0; lastSelect = true;
        resetLevel();
    }

    void update() override {
        if (state == 0) {
            bool currSelect = (digitalRead(btnSelect) == LOW);
            
            // Movement
            if (digitalRead(btnLeft) == LOW) { pX -= 1.5; }
            if (digitalRead(btnRight) == LOW) { pX += 1.5; }
            if (currSelect && !lastSelect) { pY -= 10; }
            lastSelect = currSelect;
            
            if (pX < 0) pX = 0; if (pX > 120) pX = 120;
            if (pY < 0) { // Reached Top!
                score++;
                resetLevel();
            }
            if (pY > 56) pY = 56;
            
            bool onLog = false;
            
            // Move objects & Collision
            for (int i=0; i<6; i++) {
                rows[i].x += rows[i].speed;
                if (rows[i].x > 128) rows[i].x = -rows[i].w;
                if (rows[i].x < -rows[i].w) rows[i].x = 128;
                
                // Collision Y check
                if (pY+6 > rows[i].y && pY < rows[i].y+6) {
                    // Collision X check
                    if (pX+6 > rows[i].x && pX < rows[i].x + rows[i].w) {
                        if (rows[i].isLog) {
                            onLog = true;
                            pX += rows[i].speed; // ride log
                        } else {
                            state = 1; // Hit by car
                            delay(500);
                        }
                    }
                }
            }
            
            // River check (Y between 0 and 20 roughly)
            if (pY < 20 && pY > 0 && !onLog) {
                state = 1; // Drowned
                delay(500);
            }
            
            display.clearDisplay();
            
            // Draw River
            display.fillRect(0, 0, 128, 22, WHITE);
            
            // Draw Objects
            for (int i=0; i<6; i++) {
                if (rows[i].isLog) {
                    display.fillRect((int)rows[i].x, (int)rows[i].y, rows[i].w, 6, BLACK);
                    display.drawRect((int)rows[i].x, (int)rows[i].y, rows[i].w, 6, WHITE); // outline
                } else {
                    display.fillRect((int)rows[i].x, (int)rows[i].y, rows[i].w, 6, WHITE);
                }
            }
            
            // Draw Frog
            display.fillRect((int)pX, (int)pY, 6, 6, (pY < 22) ? BLACK : WHITE); // inverse in river
            
            display.setCursor(0, 56);
            if (pY < 50) display.print(score); // hide score if frog is at bottom to avoid overlap
            
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(20, 20); display.print("SPLAT!");
            display.setCursor(20, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(FroggerGame)
