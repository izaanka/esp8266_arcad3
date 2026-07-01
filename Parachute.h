#pragma once

class ParachuteGame : public ArcadeGame {
private:
    float pX = 64; // Boat
    float heliX = 0;
    float heliDir = 1.0;
    
    struct Trooper { float x, y; bool active; };
    Trooper troopers[4];
    
    int score = 0;
    int misses = 0;
    int state = 0;

    void dropTrooper() {
        for(int i=0; i<4; i++) {
            if (!troopers[i].active) {
                troopers[i].active = true;
                troopers[i].x = heliX;
                troopers[i].y = 15;
                break;
            }
        }
    }

public:
    const char* getCategory() override { return "Arcade"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Parachute"; }

    void init() override {
        pX = 64; heliX = 0; heliDir = 1.0;
        score = 0; misses = 0; state = 0;
        for(int i=0; i<4; i++) troopers[i].active = false;
    }

    void update() override {
        if (state == 0) {
            // Boat
            if (digitalRead(btnLeft) == LOW) pX -= 3.0;
            if (digitalRead(btnRight) == LOW) pX += 3.0;
            if (pX < 0) pX = 0; if (pX > 108) pX = 108; // boat is 20 wide
            
            // Heli
            heliX += heliDir * (1.5 + (score*0.05));
            if (heliX > 120) { heliX = 120; heliDir = -1.0; }
            if (heliX < 8) { heliX = 8; heliDir = 1.0; }
            
            if (random(100) < 4) dropTrooper();
            
            // Troopers
            for(int i=0; i<4; i++) {
                if (troopers[i].active) {
                    troopers[i].y += 1.0; // fall
                    if (troopers[i].y > 54) {
                        // Check catch
                        if (troopers[i].x + 4 > pX && troopers[i].x < pX + 20) {
                            score++;
                            troopers[i].active = false;
                        } else if (troopers[i].y > 64) {
                            misses++;
                            troopers[i].active = false;
                            if (misses >= 3) {
                                state = 1;
                                delay(500);
                            }
                        }
                    }
                }
            }
            
            display.clearDisplay();
            
            // Water
            display.drawLine(0, 60, 128, 60, WHITE);
            
            // Boat
            display.drawLine((int)pX, 55, (int)pX+20, 55, WHITE);
            display.drawLine((int)pX, 55, (int)pX+4, 60, WHITE);
            display.drawLine((int)pX+20, 55, (int)pX+16, 60, WHITE);
            
            // Heli
            display.fillRect((int)heliX - 6, 8, 12, 6, WHITE);
            display.drawLine((int)heliX - 2, 8, (int)heliX - 2, 5, WHITE);
            display.drawLine((int)heliX - 10, 5, (int)heliX + 6, 5, WHITE); // blades
            display.drawLine((int)heliX + 6, 8, (int)heliX + 10, 8, WHITE); // tail
            
            // Troopers
            for(int i=0; i<4; i++) {
                if (troopers[i].active) {
                    display.drawCircle((int)troopers[i].x, (int)troopers[i].y - 6, 4, WHITE); // parachute
                    display.drawLine((int)troopers[i].x - 4, (int)troopers[i].y - 6, (int)troopers[i].x, (int)troopers[i].y, WHITE);
                    display.drawLine((int)troopers[i].x + 4, (int)troopers[i].y - 6, (int)troopers[i].x, (int)troopers[i].y, WHITE);
                    display.drawPixel((int)troopers[i].x, (int)troopers[i].y + 1, WHITE); // guy
                }
            }
            
            // UI
            display.setCursor(0, 0); display.print(score);
            display.setCursor(100, 0); 
            for(int i=0; i<3-misses; i++) display.print("X");
            
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

REGISTER_GAME(ParachuteGame)
