#pragma once

class RiverRaidGame : public ArcadeGame {
private:
    float pX = 64;
    float fuel = 100.0;
    int score = 0;
    int state = 0;
    
    struct Obj { float x, y; int type; bool active; }; // 0=heli(enemy), 1=fuel
    Obj objs[5];
    
    struct Bullet { float x, y; bool active; };
    Bullet bullets[3];
    
    float riverWidth = 80;
    float riverOffset = 0;
    float scrollY = 0;
    
    bool lastSelect = true;

    void spawnObj() {
        for(int i=0; i<5; i++) {
            if (!objs[i].active) {
                objs[i].active = true;
                objs[i].y = -10;
                objs[i].x = 64 + riverOffset + random(-30, 30);
                objs[i].type = (random(10) > 7) ? 1 : 0; // 30% fuel
                break;
            }
        }
    }

public:
    const char* getName() override { return "River Raid"; }

    void init() override {
        pX = 64; fuel = 100.0; score = 0; state = 0; riverOffset = 0; scrollY = 0; lastSelect = true;
        for(int i=0; i<5; i++) objs[i].active = false;
        for(int i=0; i<3; i++) bullets[i].active = false;
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) pX -= 2.0;
            if (digitalRead(btnRight) == LOW) pX += 2.0;
            
            bool cS = (digitalRead(btnSelect) == LOW);
            if (cS && !lastSelect) {
                for(int i=0; i<3; i++) {
                    if (!bullets[i].active) {
                        bullets[i].active = true;
                        bullets[i].x = pX;
                        bullets[i].y = 50;
                        break;
                    }
                }
            }
            lastSelect = cS;
            
            // Fuel
            fuel -= 0.1;
            if (fuel <= 0) { state = 1; delay(500); }
            
            // Scroll
            scrollY += 1.0;
            riverOffset = sin(scrollY * 0.02) * 20;
            
            if (random(100) < 4) spawnObj();
            
            // Move Objs & Collision
            for(int i=0; i<5; i++) {
                if (objs[i].active) {
                    objs[i].y += 1.0; // scroll speed
                    
                    if (objs[i].y > 64) objs[i].active = false;
                    
                    // Collision with player
                    if (abs(objs[i].x - pX) < 8 && abs(objs[i].y - 52) < 8) {
                        if (objs[i].type == 1) { // Fuel
                            fuel += 20;
                            if (fuel > 100) fuel = 100;
                            objs[i].active = false;
                        } else { // Enemy
                            state = 1;
                            delay(500);
                        }
                    }
                }
            }
            
            // Move Bullets
            for(int i=0; i<3; i++) {
                if (bullets[i].active) {
                    bullets[i].y -= 4.0;
                    if (bullets[i].y < 0) bullets[i].active = false;
                    
                    for(int j=0; j<5; j++) {
                        if (objs[j].active) {
                            if (abs(bullets[i].x - objs[j].x) < 8 && abs(bullets[i].y - objs[j].y) < 6) {
                                objs[j].active = false;
                                bullets[i].active = false;
                                if (objs[j].type == 0) score += 10; // only get points for enemies, destroying fuel is bad
                            }
                        }
                    }
                }
            }
            
            // Wall Collision
            float leftBank = 64 + riverOffset - riverWidth/2;
            float rightBank = 64 + riverOffset + riverWidth/2;
            if (pX < leftBank + 4 || pX > rightBank - 4) {
                state = 1; // Crashed into bank
                delay(500);
            }
            
            display.clearDisplay();
            
            // Banks (draw dots for terrain)
            for(int y=0; y<64; y+=4) {
                float ro = sin((scrollY - y) * 0.02) * 20;
                display.drawLine(0, y, 64 + ro - riverWidth/2, y, WHITE);
                display.drawLine(64 + ro + riverWidth/2, y, 128, y, WHITE);
            }
            
            // Plane
            display.fillRect((int)pX - 1, 50, 3, 6, WHITE); // fuselage
            display.drawLine((int)pX - 6, 53, (int)pX + 6, 53, WHITE); // wings
            display.drawLine((int)pX - 3, 56, (int)pX + 3, 56, WHITE); // tail
            
            // Objs
            for(int i=0; i<5; i++) {
                if (objs[i].active) {
                    if (objs[i].type == 0) { // Heli
                        display.drawRect((int)objs[i].x - 4, (int)objs[i].y - 3, 8, 6, WHITE);
                        display.drawLine((int)objs[i].x - 6, (int)objs[i].y - 3, (int)objs[i].x + 6, (int)objs[i].y - 3, WHITE); // blade
                    } else { // Fuel
                        display.fillRect((int)objs[i].x - 3, (int)objs[i].y - 4, 6, 8, WHITE);
                        display.setCursor((int)objs[i].x - 2, (int)objs[i].y - 3);
                        display.setTextColor(BLACK, WHITE);
                        display.print("F");
                        display.setTextColor(WHITE, BLACK);
                    }
                }
            }
            
            // Bullets
            for(int i=0; i<3; i++) {
                if (bullets[i].active) display.drawLine((int)bullets[i].x, (int)bullets[i].y, (int)bullets[i].x, (int)bullets[i].y-2, WHITE);
            }
            
            // UI
            display.setCursor(0,0); display.print(score);
            display.drawRect(30, 0, 40, 4, WHITE);
            display.fillRect(30, 0, (int)(fuel * 0.4), 4, WHITE);
            
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("CRASHED!");
            display.setCursor(35, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(RiverRaidGame)
