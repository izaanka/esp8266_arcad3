#pragma once

class HillClimbGame : public ArcadeGame {
private:
    float angle, aVel;
    float dist;
    float speed;
    float terrainOffset;
    int score;
    int state;

    float getTerrainY(float x) {
        return 40 + sin((x + terrainOffset) * 0.05) * 15 + cos((x + terrainOffset) * 0.1) * 5;
    }

public:
    const char* getCategory() override { return "Sports"; }
    const char* getName() override { return "Hill Climb"; }
    int getScore() override { return score; }

    void init() override {
        angle = 0; aVel = 0; dist = 0; speed = 0; terrainOffset = 0;
        score = 0; state = 0;
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW); // Brake / tilt back
            bool cR = (digitalRead(btnRight) == LOW); // Gas / tilt forward

            if (cR) {
                speed += 0.2;
                aVel += 0.02; // torque makes front lift
            }
            if (cL) {
                speed -= 0.3;
                aVel -= 0.02; // braking makes front dip
            }
            
            speed -= 0.05; // friction
            if (speed < -2.0) speed = -2.0;
            if (speed > 4.0) speed = 4.0;

            float terrainSlope = atan2(getTerrainY(64 + 10) - getTerrainY(64 - 10), 20);
            
            // Gravity pulls car towards terrain slope
            float diff = terrainSlope - angle;
            aVel += diff * 0.05;
            aVel *= 0.9; // dampening
            angle += aVel;

            if (abs(angle) > PI/2) {
                state = 1; delay(500); // Flipped!
            }

            terrainOffset += speed;
            dist += speed;
            score = (int)(dist / 10.0);

            display.clearDisplay();
            
            // Terrain
            for(int x=0; x<128; x+=4) {
                display.drawLine(x, (int)getTerrainY(x), x+4, (int)getTerrainY(x+4), WHITE);
            }

            // Car
            float cy = getTerrainY(64) - 5;
            // Draw relative to angle
            int dx = cos(angle) * 8; int dy = sin(angle) * 8;
            display.drawLine(64 - dx, cy - dy, 64 + dx, cy + dy, WHITE); // chassis
            display.fillRect(64 - 4, cy - dy - 6, 8, 6, WHITE); // cab
            
            display.setCursor(0,0); display.print("Dist:"); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(40, 20); display.print("FLIPPED!");
            display.setCursor(40, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(HillClimbGame)
