#pragma once

class InvadersGame : public ArcadeGame {
private:
    float pX = 64;
    int score = 0;
    int state = 0;
    
    struct Alien { float x, y; bool active; };
    Alien aliens[15]; // 5 cols x 3 rows
    
    float aDir = 1.0;
    float aSpeed = 0.5;
    
    struct Bullet { float x, y, vy; bool active; };
    Bullet pBullets[2];
    Bullet aBullets[3];
    
    bool lastSelect = true;

    void resetAliens() {
        for(int r=0; r<3; r++) {
            for(int c=0; c<5; c++) {
                int i = r*5 + c;
                aliens[i].x = 10 + c*15;
                aliens[i].y = 10 + r*12;
                aliens[i].active = true;
            }
        }
        aDir = 1.0;
    }

public:
    const char* getCategory() override { return "Arcade"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Space Invaders"; }

    void init() override {
        pX = 64; score = 0; state = 0; aSpeed = 0.5; lastSelect = true;
        resetAliens();
        for(int i=0; i<2; i++) pBullets[i].active = false;
        for(int i=0; i<3; i++) aBullets[i].active = false;
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) pX -= 2.0;
            if (digitalRead(btnRight) == LOW) pX += 2.0;
            if (pX < 4) pX = 4; if (pX > 123) pX = 123;
            
            bool currSelect = (digitalRead(btnSelect) == LOW);
            if (currSelect && !lastSelect) {
                for(int i=0; i<2; i++) {
                    if (!pBullets[i].active) {
                        pBullets[i].active = true;
                        pBullets[i].x = pX;
                        pBullets[i].y = 55;
                        pBullets[i].vy = -3.0;
                        break;
                    }
                }
            }
            lastSelect = currSelect;
            
            // Move Aliens
            bool edge = false;
            int activeCount = 0;
            for(int i=0; i<15; i++) {
                if (aliens[i].active) {
                    activeCount++;
                    aliens[i].x += aDir * aSpeed;
                    if (aliens[i].x > 120 || aliens[i].x < 8) edge = true;
                    
                    // Alien Shoot
                    if (random(1000) < 5) {
                        for(int b=0; b<3; b++) {
                            if (!aBullets[b].active) {
                                aBullets[b].active = true;
                                aBullets[b].x = aliens[i].x;
                                aBullets[b].y = aliens[i].y + 4;
                                aBullets[b].vy = 2.0;
                                break;
                            }
                        }
                    }
                }
            }
            
            if (activeCount == 0) {
                aSpeed += 0.2;
                resetAliens();
            } else if (edge) {
                aDir = -aDir;
                for(int i=0; i<15; i++) {
                    if (aliens[i].active) {
                        aliens[i].x += aDir * aSpeed * 2; // back off edge
                        aliens[i].y += 4;
                        if (aliens[i].y >= 55) { state = 1; delay(500); } // Invaded
                    }
                }
            }
            
            // Move Bullets & Check Collisions
            for(int i=0; i<2; i++) {
                if (pBullets[i].active) {
                    pBullets[i].y += pBullets[i].vy;
                    if (pBullets[i].y < 0) pBullets[i].active = false;
                    else {
                        for(int a=0; a<15; a++) {
                            if (aliens[a].active) {
                                if (abs(pBullets[i].x - aliens[a].x) < 5 && abs(pBullets[i].y - aliens[a].y) < 4) {
                                    aliens[a].active = false;
                                    pBullets[i].active = false;
                                    score += 10;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            
            for(int i=0; i<3; i++) {
                if (aBullets[i].active) {
                    aBullets[i].y += aBullets[i].vy;
                    if (aBullets[i].y > 64) aBullets[i].active = false;
                    else {
                        if (abs(aBullets[i].x - pX) < 4 && aBullets[i].y > 55) {
                            state = 1; // Hit
                            delay(500);
                        }
                    }
                }
            }
            
            display.clearDisplay();
            
            // Player
            display.fillRect((int)pX - 3, 58, 7, 4, WHITE);
            display.drawLine((int)pX, 58, (int)pX, 56, WHITE);
            
            // Aliens
            for(int i=0; i<15; i++) {
                if (aliens[i].active) {
                    display.fillRect((int)aliens[i].x - 3, (int)aliens[i].y - 2, 7, 5, WHITE);
                    display.drawPixel((int)aliens[i].x - 1, (int)aliens[i].y - 1, BLACK); // eyes
                    display.drawPixel((int)aliens[i].x + 1, (int)aliens[i].y - 1, BLACK);
                }
            }
            
            // Bullets
            for(int i=0; i<2; i++) if(pBullets[i].active) display.drawLine((int)pBullets[i].x, (int)pBullets[i].y, (int)pBullets[i].x, (int)pBullets[i].y-2, WHITE);
            for(int i=0; i<3; i++) if(aBullets[i].active) display.drawPixel((int)aBullets[i].x, (int)aBullets[i].y, WHITE);
            
            display.setCursor(0,0); display.print(score);
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("GAME OVER");
            display.setCursor(30, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(InvadersGame)
