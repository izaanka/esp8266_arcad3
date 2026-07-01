#pragma once

class DuckHuntGame : public ArcadeGame {
private:
    float cX = 64; // Crosshair X
    int score = 0;
    int state = 0;
    
    struct Duck { float x, y, vx, vy; int type; bool active; };
    Duck ducks[3];
    
    int bullets = 3;
    bool lastSelect = true;

    void spawnDuck() {
        for(int i=0; i<3; i++) {
            if (!ducks[i].active) {
                ducks[i].active = true;
                ducks[i].y = random(10, 40);
                if (random(2) == 0) { ducks[i].x = -10; ducks[i].vx = random(15, 30)/10.0; }
                else { ducks[i].x = 138; ducks[i].vx = -random(15, 30)/10.0; }
                ducks[i].vy = random(-5, 5)/10.0;
                ducks[i].type = 0;
                break;
            }
        }
    }

public:
    const char* getCategory() override { return "Arcade"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Duck Hunt"; }

    void init() override {
        cX = 64; score = 0; state = 0; bullets = 3; lastSelect = true;
        for(int i=0; i<3; i++) ducks[i].active = false;
        spawnDuck();
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) cX -= 3.0;
            if (digitalRead(btnRight) == LOW) cX += 3.0;
            if (cX < 0) cX = 0; if (cX > 128) cX = 128;
            
            bool currSelect = (digitalRead(btnSelect) == LOW);
            if (currSelect && !lastSelect && bullets > 0) {
                bullets--;
                // Fire! Shoot straight up from cX
                bool hit = false;
                for(int i=0; i<3; i++) {
                    if (ducks[i].active && ducks[i].type == 0) {
                        if (abs(ducks[i].x - cX) < 8) {
                            ducks[i].type = 1; // Falling
                            ducks[i].vy = 3.0;
                            ducks[i].vx = 0;
                            score++;
                            hit = true;
                            bullets = 3; // reload on hit for arcade style, or maybe not. 
                        }
                    }
                }
                
                // Draw flash
                display.drawLine(cX, 64, cX, 0, WHITE);
            } else if (currSelect && !lastSelect && bullets <= 0) {
                // Out of bullets
                state = 1; 
                delay(500);
            }
            lastSelect = currSelect;
            
            if (random(100) < 3) spawnDuck();
            
            int activeCount = 0;
            for(int i=0; i<3; i++) {
                if (ducks[i].active) {
                    activeCount++;
                    ducks[i].x += ducks[i].vx;
                    ducks[i].y += ducks[i].vy;
                    
                    if (ducks[i].type == 0) { // Flying
                        if (ducks[i].x < -20 || ducks[i].x > 148 || ducks[i].y < -10) ducks[i].active = false;
                    } else { // Falling
                        if (ducks[i].y > 64) {
                            ducks[i].active = false;
                        }
                    }
                }
            }
            
            // If all ducks got away and out of bullets
            if (activeCount == 0 && bullets <= 0) {
                state = 1;
                delay(500);
            }
            
            display.clearDisplay();
            
            // Draw Ducks
            for(int i=0; i<3; i++) {
                if (ducks[i].active) {
                    if (ducks[i].type == 0) {
                        display.fillRect((int)ducks[i].x - 4, (int)ducks[i].y - 2, 8, 4, WHITE);
                        display.drawLine((int)ducks[i].x, (int)ducks[i].y, (int)ducks[i].x + ((ducks[i].vx>0)?4:-4), (int)ducks[i].y-2, WHITE); // head
                        if ((millis()/100)%2 == 0) display.drawLine((int)ducks[i].x, (int)ducks[i].y, (int)ducks[i].x-2, (int)ducks[i].y-4, WHITE); // wing
                    } else {
                        // Falling
                        display.drawRect((int)ducks[i].x - 4, (int)ducks[i].y - 4, 8, 8, WHITE);
                        display.drawLine((int)ducks[i].x - 4, (int)ducks[i].y - 4, (int)ducks[i].x + 4, (int)ducks[i].y + 4, WHITE);
                    }
                }
            }
            
            // Draw Crosshair (just an indicator at bottom)
            display.drawLine(cX, 58, cX, 64, WHITE);
            display.drawLine(cX-3, 61, cX+3, 61, WHITE);
            
            // UI
            display.setCursor(0,0); display.print(score);
            display.setCursor(100,0); 
            for(int i=0; i<bullets; i++) display.print("|");
            
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

REGISTER_GAME(DuckHuntGame)
