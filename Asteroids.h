#pragma once

class AsteroidsGame : public ArcadeGame {
private:
    float pX, pY, pVX, pVY;
    float pAngle;
    
    struct Bullet { float x, y, vx, vy; bool active; };
    Bullet bullets[5];
    
    struct Asteroid { float x, y, vx, vy; int size; bool active; };
    Asteroid rocks[8];
    
    int score = 0;
    int state = 0; // 0=play, 1=gameover
    bool lastSelect = true;

    void spawnRock() {
        for (int i = 0; i < 8; i++) {
            if (!rocks[i].active) {
                rocks[i].active = true;
                rocks[i].size = random(2, 4); // 2=small, 3=medium
                
                // Spawn on edges
                if (random(2) == 0) {
                    rocks[i].x = (random(2) == 0) ? 0 : 128;
                    rocks[i].y = random(0, 64);
                } else {
                    rocks[i].x = random(0, 128);
                    rocks[i].y = (random(2) == 0) ? 0 : 64;
                }
                
                float angle = random(0, 360) * PI / 180.0;
                float speed = random(5, 15) / 10.0;
                rocks[i].vx = cos(angle) * speed;
                rocks[i].vy = sin(angle) * speed;
                break;
            }
        }
    }

public:
    const char* getName() override { return "Asteroids"; }

    void init() override {
        pX = 64; pY = 32; pVX = 0; pVY = 0; pAngle = -PI/2;
        score = 0; state = 0; lastSelect = true;
        for (int i=0; i<5; i++) bullets[i].active = false;
        for (int i=0; i<8; i++) rocks[i].active = false;
        
        spawnRock(); spawnRock(); spawnRock();
    }

    void update() override {
        if (state == 0) {
            // Controls
            if (digitalRead(btnLeft) == LOW) pAngle -= 0.15;
            if (digitalRead(btnRight) == LOW) pAngle += 0.15;
            
            bool currSelect = (digitalRead(btnSelect) == LOW);
            if (currSelect && !lastSelect) { // Fire
                for (int i=0; i<5; i++) {
                    if (!bullets[i].active) {
                        bullets[i].active = true;
                        bullets[i].x = pX + cos(pAngle)*6;
                        bullets[i].y = pY + sin(pAngle)*6;
                        bullets[i].vx = cos(pAngle)*4.0;
                        bullets[i].vy = sin(pAngle)*4.0;
                        break;
                    }
                }
            }
            if (currSelect) { // Thrust
                pVX += cos(pAngle) * 0.1;
                pVY += sin(pAngle) * 0.1;
            }
            lastSelect = currSelect;
            
            // Physics Ship
            pX += pVX; pY += pVY;
            pVX *= 0.98; pVY *= 0.98; // Friction
            
            if (pX < 0) pX += 128; if (pX > 128) pX -= 128;
            if (pY < 0) pY += 64; if (pY > 64) pY -= 64;
            
            // Physics Bullets
            for (int i=0; i<5; i++) {
                if (bullets[i].active) {
                    bullets[i].x += bullets[i].vx;
                    bullets[i].y += bullets[i].vy;
                    if (bullets[i].x < 0 || bullets[i].x > 128 || bullets[i].y < 0 || bullets[i].y > 64) {
                        bullets[i].active = false;
                    }
                }
            }
            
            // Physics Rocks
            if (random(0, 100) < 2) spawnRock();
            
            for (int i=0; i<8; i++) {
                if (rocks[i].active) {
                    rocks[i].x += rocks[i].vx;
                    rocks[i].y += rocks[i].vy;
                    if (rocks[i].x < -10) rocks[i].x = 138;
                    if (rocks[i].x > 138) rocks[i].x = -10;
                    if (rocks[i].y < -10) rocks[i].y = 74;
                    if (rocks[i].y > 74) rocks[i].y = -10;
                    
                    // Collision with Bullets
                    for (int b=0; b<5; b++) {
                        if (bullets[b].active) {
                            float dx = bullets[b].x - rocks[i].x;
                            float dy = bullets[b].y - rocks[i].y;
                            if (dx*dx + dy*dy < rocks[i].size*4 * rocks[i].size*4) {
                                bullets[b].active = false;
                                rocks[i].active = false;
                                score += 10;
                            }
                        }
                    }
                    
                    // Collision with Ship
                    if (rocks[i].active) {
                        float dx = pX - rocks[i].x;
                        float dy = pY - rocks[i].y;
                        if (dx*dx + dy*dy < (rocks[i].size*3 + 3) * (rocks[i].size*3 + 3)) {
                            state = 1;
                            delay(500);
                        }
                    }
                }
            }
            
            // Draw
            display.clearDisplay();
            
            // Ship
            int p1x = pX + cos(pAngle)*6; int p1y = pY + sin(pAngle)*6;
            int p2x = pX + cos(pAngle+2.5)*5; int p2y = pY + sin(pAngle+2.5)*5;
            int p3x = pX + cos(pAngle-2.5)*5; int p3y = pY + sin(pAngle-2.5)*5;
            display.drawTriangle(p1x, p1y, p2x, p2y, p3x, p3y, WHITE);
            if (currSelect && (millis()%100 < 50)) { // thrust flame
                display.drawLine(pX, pY, pX - cos(pAngle)*5, pY - sin(pAngle)*5, WHITE);
            }
            
            // Bullets
            for (int i=0; i<5; i++) {
                if (bullets[i].active) display.drawPixel((int)bullets[i].x, (int)bullets[i].y, WHITE);
            }
            
            // Rocks
            for (int i=0; i<8; i++) {
                if (rocks[i].active) {
                    display.drawCircle((int)rocks[i].x, (int)rocks[i].y, rocks[i].size*3, WHITE);
                }
            }
            
            display.setCursor(0,0); display.print(score);
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

REGISTER_GAME(AsteroidsGame)
