#pragma once

class LunarLanderGame : public ArcadeGame {
private:
    float pX, pY, pVX, pVY, pAngle;
    int fuel = 100;
    int score = 0;
    int state = 0; // 0=play, 1=win, 2=crash
    
    float terrainY[9];
    int padIndex;
    
    void generateTerrain() {
        padIndex = random(1, 7); // pad is between x=16 and x=112
        float y = random(40, 60);
        for(int i=0; i<9; i++) {
            if (i == padIndex || i == padIndex+1) {
                terrainY[i] = y; // flat pad
            } else {
                terrainY[i] = random(35, 63);
            }
        }
    }

    float getTerrainY(float x) {
        if (x < 0) x = 0; if (x > 127) x = 127;
        int idx = (int)(x / 16);
        float t = (x - (idx * 16)) / 16.0;
        return terrainY[idx] * (1 - t) + terrainY[idx+1] * t;
    }

public:
    const char* getCategory() override { return "Arcade"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Lunar Lander"; }

    void init() override {
        pX = 10; pY = 10; pVX = 1.0; pVY = 0; pAngle = 0;
        fuel = 200; state = 0;
        generateTerrain();
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) pAngle -= 0.1;
            if (digitalRead(btnRight) == LOW) pAngle += 0.1;
            
            bool thrust = false;
            if (digitalRead(btnSelect) == LOW && fuel > 0) {
                pVX += sin(pAngle) * 0.1;
                pVY -= cos(pAngle) * 0.1; // Thrust is UP (negative Y)
                fuel--;
                thrust = true;
            }
            
            pVY += 0.04; // Gravity
            pX += pVX; pY += pVY;
            
            // Screen limits
            if (pX < 0) { pX = 0; pVX = -pVX; }
            if (pX > 128) { pX = 128; pVX = -pVX; }
            if (pY < 0) { pY = 0; pVY = 0; }
            
            float groundY = getTerrainY(pX);
            
            if (pY >= groundY) { // Ship bottom is visually drawn at pY
                // Collision!
                // Check if landed safely on pad
                bool onPad = (pX >= padIndex * 16 && pX <= (padIndex+1) * 16);
                bool upright = (abs(pAngle) < 0.3); // made slightly easier
                bool slow = (pVY < 1.5 && abs(pVX) < 1.0); // made slightly easier
                
                if (onPad && upright && slow) {
                    state = 1; // Win
                    score += fuel;
                } else {
                    state = 2; // Crash
                }
                delay(500);
            }
            
            display.clearDisplay();
            
            // Terrain
            for(int i=0; i<8; i++) {
                display.drawLine(i*16, (int)terrainY[i], (i+1)*16, (int)terrainY[i+1], WHITE);
                if (i == padIndex) { // draw pad thicker
                    display.drawLine(i*16, (int)terrainY[i]+1, (i+1)*16, (int)terrainY[i+1]+1, WHITE);
                }
            }
            
            // Ship (visually rests at pY)
            display.drawLine(pX, pY, pX + sin(pAngle)*6, pY - cos(pAngle)*6, WHITE); // nose
            display.drawLine(pX - cos(pAngle)*3, pY - sin(pAngle)*3, pX + cos(pAngle)*3, pY + sin(pAngle)*3, WHITE); // base
            
            if (thrust && (millis()%100 < 50)) {
                display.drawPixel(pX - sin(pAngle)*3, pY + cos(pAngle)*3, WHITE);
            }
            
            // HUD
            display.setCursor(0,0); display.print("F:"); display.print(fuel);
            display.setCursor(64,0); display.print("S:"); display.print(score);
            
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); 
            if (state == 1) display.print("LANDED!"); else display.print("CRASHED!");
            display.setCursor(20, 35); display.print("Score: "); display.print(score);
            display.setCursor(20, 50); display.print("D6: Try Again");
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(LunarLanderGame)
