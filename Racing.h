#pragma once

class RacingGame : public ArcadeGame {
private:
    float pX = 64;
    int score = 0;
    int state = 0;
    
    struct Car { float x, y; bool active; };
    Car enemies[3];
    
    float roadOffset = 0;
    float roadCurve = 0;
    float speed = 2.0;

    void spawnEnemy() {
        for(int i=0; i<3; i++) {
            if (!enemies[i].active) {
                enemies[i].active = true;
                enemies[i].y = -20;
                enemies[i].x = 64 + roadOffset + random(-20, 20);
                break;
            }
        }
    }

public:
    const char* getName() override { return "Highway Racing"; }

    void init() override {
        pX = 64; score = 0; state = 0;
        speed = 2.0; roadOffset = 0; roadCurve = 0;
        for(int i=0; i<3; i++) enemies[i].active = false;
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) pX -= 3.0;
            if (digitalRead(btnRight) == LOW) pX += 3.0;
            
            roadCurve += 0.05;
            roadOffset = sin(roadCurve) * 20; // Road bends left and right
            
            if (random(100) < 5) spawnEnemy();
            
            // Move enemies
            for(int i=0; i<3; i++) {
                if (enemies[i].active) {
                    enemies[i].y += speed * 0.8; // Enemies move slower than road (so they approach you)
                    // Road movement logic: objects on road should also sway?
                    // Simplified: enemies just move straight down. 
                    if (enemies[i].y > 64) {
                        enemies[i].active = false;
                        score++;
                        if (score % 5 == 0) speed += 0.5;
                    }
                    
                    // Collision
                    if (abs(pX - enemies[i].x) < 8 && abs(50 - enemies[i].y) < 10) {
                        state = 1;
                        delay(500);
                    }
                }
            }
            
            // Check off road
            if (pX < 64 + roadOffset - 30 || pX > 64 + roadOffset + 30) {
                state = 1; // crashed into grass
                delay(500);
            }
            
            display.clearDisplay();
            
            // Draw Road
            for (int y=0; y<64; y+=8) {
                float ro = sin(roadCurve - (y*0.01)) * 20;
                display.drawPixel(64 + ro - 30, y, WHITE);
                display.drawPixel(64 + ro + 30, y, WHITE);
                if ((y + (int)(score*10)) % 16 < 8) { // center dashed line
                    display.drawPixel(64 + ro, y, WHITE);
                    display.drawPixel(64 + ro, y+1, WHITE);
                }
            }
            
            // Enemies
            for(int i=0; i<3; i++) {
                if (enemies[i].active) {
                    display.fillRect((int)enemies[i].x - 4, (int)enemies[i].y - 6, 8, 12, WHITE);
                    display.fillRect((int)enemies[i].x - 2, (int)enemies[i].y - 4, 4, 8, BLACK); // detail
                }
            }
            
            // Player
            display.drawRect((int)pX - 4, 44, 8, 12, WHITE);
            display.fillRect((int)pX - 2, 46, 4, 8, WHITE);
            
            display.setCursor(0,0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("CRASHED!");
            display.setCursor(30, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(RacingGame)
