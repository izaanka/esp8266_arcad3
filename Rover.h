#pragma once

class RoverGame : public ArcadeGame {
private:
    float angle = 0; // 0 is flat. positive is pitched up.
    float rX = 64, rY = 32;
    float rVY = 0;
    
    float terrainY[17];
    float offset = 0;
    float speed = 1.0;
    int score = 0;
    int state = 0;
    
    void generateTerrain() {
        terrainY[0] = 50;
        for(int i=1; i<17; i++) {
            terrainY[i] = terrainY[i-1] + random(-5, 5);
            if (terrainY[i] < 20) terrainY[i] = 20;
            if (terrainY[i] > 60) terrainY[i] = 60;
        }
    }

public:
    const char* getCategory() override { return "Arcade"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Mars Rover"; }

    void init() override {
        angle = 0; rY = 20; rVY = 0; offset = 0; speed = 1.0; score = 0; state = 0;
        generateTerrain();
    }

    void update() override {
        if (state == 0) {
            // Control rotation
            if (digitalRead(btnLeft) == LOW) angle -= 0.1; // pitch down
            if (digitalRead(btnRight) == LOW) angle += 0.1; // pitch up
            
            // Physics
            rVY += 0.2; // gravity
            rY += rVY;
            
            // Terrain collision
            int tIdx = (64 + (int)offset) / 8;
            float t = ((64 + (int)offset) % 8) / 8.0;
            float groundY = terrainY[tIdx] * (1-t) + terrainY[tIdx+1] * t;
            
            if (rY + 4 >= groundY) { // Wheel hits ground
                rY = groundY - 4;
                rVY = 0;
                
                // If angle is too extreme, crash
                if (angle > PI/2 || angle < -PI/2) {
                    state = 1;
                    delay(500);
                } else {
                    // Match terrain angle slightly (suspension)
                    float targetAngle = atan2(terrainY[tIdx+1] - terrainY[tIdx], 8);
                    angle += (targetAngle - angle) * 0.1; // auto level to terrain
                }
            }
            
            // Scrolling
            offset += speed;
            if (offset >= 8) {
                offset -= 8;
                score++;
                if (score % 200 == 0) speed += 0.2;
                
                for(int i=0; i<16; i++) terrainY[i] = terrainY[i+1];
                
                float nextY = terrainY[15] + random(-8, 8);
                if (nextY < 20) nextY = 20;
                if (nextY > 60) nextY = 60;
                terrainY[16] = nextY;
            }
            
            display.clearDisplay();
            
            // Terrain
            for(int i=0; i<16; i++) {
                int x1 = i*8 - (int)offset;
                int x2 = (i+1)*8 - (int)offset;
                display.drawLine(x1, (int)terrainY[i], x2, (int)terrainY[i+1], WHITE);
            }
            
            // Rover Body
            // 4 points rotated by angle
            float w = 12, h = 6;
            float pts[4][2] = {
                {-w, -h}, {w, -h}, {w, h}, {-w, h}
            };
            
            int screenPts[4][2];
            for(int i=0; i<4; i++) {
                float rx = pts[i][0] * cos(angle) - pts[i][1] * sin(angle);
                float ry = pts[i][0] * sin(angle) + pts[i][1] * cos(angle);
                screenPts[i][0] = rX + rx;
                screenPts[i][1] = rY + ry - 4; // Shift up a bit
            }
            
            for(int i=0; i<4; i++) {
                display.drawLine(screenPts[i][0], screenPts[i][1], screenPts[(i+1)%4][0], screenPts[(i+1)%4][1], WHITE);
            }
            
            // Wheels
            float wx1 = -8 * cos(angle) - 4 * sin(angle);
            float wy1 = -8 * sin(angle) + 4 * cos(angle);
            float wx2 = 8 * cos(angle) - 4 * sin(angle);
            float wy2 = 8 * sin(angle) + 4 * cos(angle);
            
            display.drawCircle((int)(rX + wx1), (int)(rY + wy1), 3, WHITE);
            display.drawCircle((int)(rX + wx2), (int)(rY + wy2), 3, WHITE);
            
            display.setCursor(0,0); display.print(score/10);
            
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("FLIPPED!");
            display.setCursor(35, 35); display.print("Dist: "); display.print(score/10);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(RoverGame)
