#pragma once

class ArtilleryGame : public ArcadeGame {
private:
    float angle = PI / 4;
    float power = 0;
    bool charging = false;
    
    struct Shell { float x, y, vx, vy; bool active; };
    Shell shell;
    
    float terrainY[16];
    float targetX, targetY;
    
    int score = 0;
    int state = 0; // 0=aim, 1=shoot, 2=over
    bool lastSelect = true;

    void generateTerrain() {
        // Flat start
        terrainY[0] = 50; terrainY[1] = 50;
        float currY = 50;
        for(int i=2; i<16; i++) {
            currY += random(-8, 8);
            if (currY < 20) currY = 20;
            if (currY > 60) currY = 60;
            terrainY[i] = currY;
        }
        
        // Place target
        int tIdx = random(8, 15);
        targetX = tIdx * 8 + 4;
        targetY = terrainY[tIdx];
    }
    
    float getTerrainY(float x) {
        if (x < 0) x = 0; if (x > 127) x = 127;
        int idx = (int)(x / 8.0);
        float t = (x - (idx * 8)) / 8.0;
        if (idx >= 15) return terrainY[15];
        return terrainY[idx] * (1 - t) + terrainY[idx+1] * t;
    }

public:
    const char* getName() override { return "Artillery"; }

    void init() override {
        score = 0; state = 0; angle = PI / 4; power = 0; charging = false;
        shell.active = false; lastSelect = true;
        generateTerrain();
    }

    void update() override {
        if (state == 0) {
            // Aiming & Power
            if (digitalRead(btnLeft) == LOW) angle += 0.05; // higher
            if (digitalRead(btnRight) == LOW) angle -= 0.05; // lower
            
            if (angle < 0.1) angle = 0.1;
            if (angle > PI/2 - 0.1) angle = PI/2 - 0.1;
            
            bool currSelect = (digitalRead(btnSelect) == LOW);
            if (currSelect) {
                charging = true;
                power += 0.1;
                if (power > 10.0) power = 10.0;
            } else if (!currSelect && lastSelect && charging) {
                // Fire!
                shell.active = true;
                shell.x = 8 + cos(angle)*6;
                shell.y = 50 - sin(angle)*6;
                shell.vx = cos(angle) * power;
                shell.vy = -sin(angle) * power;
                
                charging = false;
                power = 0;
                state = 1;
            }
            lastSelect = currSelect;
            
        } else if (state == 1) {
            // Flying Shell
            shell.vy += 0.15; // gravity
            shell.x += shell.vx;
            shell.y += shell.vy;
            
            float groundY = getTerrainY(shell.x);
            
            if (shell.y >= groundY) {
                // Hit ground
                float dx = shell.x - targetX;
                float dy = shell.y - targetY;
                if (sqrt(dx*dx + dy*dy) < 8) { // Hit target!
                    score++;
                    generateTerrain();
                }
                shell.active = false;
                state = 0; // ready for next shot
            }
            if (shell.x > 128 || shell.y > 64) {
                shell.active = false;
                state = 0;
            }
        }
        
        display.clearDisplay();
        
        // Draw Terrain
        for(int i=0; i<15; i++) {
            display.drawLine(i*8, (int)terrainY[i], (i+1)*8, (int)terrainY[i+1], WHITE);
        }
        
        // Draw Cannon
        display.fillRect(4, 48, 8, 4, WHITE);
        display.drawLine(8, 50, 8 + cos(angle)*10, 50 - sin(angle)*10, WHITE);
        
        // Draw Target
        display.fillRect((int)targetX - 3, (int)targetY - 4, 6, 4, WHITE);
        display.drawLine((int)targetX, (int)targetY - 4, (int)targetX, (int)targetY - 8, WHITE); // flag pole
        
        // Draw Power Bar
        if (charging) {
            display.drawRect(20, 10, 40, 6, WHITE);
            display.fillRect(20, 10, (int)(power * 4), 6, WHITE);
        }
        
        // Draw Shell
        if (shell.active) {
            display.fillCircle((int)shell.x, (int)shell.y, 1, WHITE);
        }
        
        display.setCursor(0,0); display.print(score);
        
        display.display();
        delay(15);
    }
};

REGISTER_GAME(ArtilleryGame)
