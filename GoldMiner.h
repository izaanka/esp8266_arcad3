#pragma once

class GoldMinerGame : public ArcadeGame {
private:
    float angle;
    int dir;
    float length;
    int state; // 0=swing, 1=shoot, 2=retract
    int score;
    
    struct Gold { int x, y; bool active; };
    Gold nuggets[4];
    int grabbedIdx;

public:
    const char* getCategory() override { return "Action"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Gold Miner"; }

    void init() override {
        angle = 0; dir = 1; length = 10; state = 0; score = 0;
        grabbedIdx = -1;
        for(int i=0; i<4; i++) {
            nuggets[i].x = random(10, 118);
            nuggets[i].y = random(20, 60);
            nuggets[i].active = true;
        }
    }

    void update() override {
        if (state == 0) {
            angle += dir * 0.05;
            if (angle > PI/2.2 || angle < -PI/2.2) dir = -dir;
            if (digitalRead(btnSelect) == LOW) { state = 1; delay(200); }
        } else if (state == 1) { // Shooting
            length += 3.0;
            float px = 64 + sin(angle) * length;
            float py = 5 + cos(angle) * length;
            
            if (px < 0 || px > 128 || py > 64) state = 2; // hit wall
            
            for(int i=0; i<4; i++) {
                if (nuggets[i].active) {
                    float dx = px - nuggets[i].x;
                    float dy = py - nuggets[i].y;
                    if (dx*dx + dy*dy < 25) {
                        nuggets[i].active = false;
                        grabbedIdx = i;
                        state = 2;
                        break;
                    }
                }
            }
        } else if (state == 2) { // Retracting
            length -= (grabbedIdx != -1) ? 1.0 : 3.0; // slower if grabbed
            if (length <= 10) {
                length = 10; state = 0;
                if (grabbedIdx != -1) { score += 10; grabbedIdx = -1; }
                
                // Check win
                bool any = false;
                for(int i=0; i<4; i++) if (nuggets[i].active) any = true;
                if (!any) {
                    for(int i=0; i<4; i++) {
                        nuggets[i].x = random(10, 118);
                        nuggets[i].y = random(20, 60);
                        nuggets[i].active = true;
                    }
                }
            }
        }

        display.clearDisplay();
        
        display.fillRect(58, 0, 12, 5, WHITE); // Machine
        
        float px = 64 + sin(angle) * length;
        float py = 5 + cos(angle) * length;
        display.drawLine(64, 5, (int)px, (int)py, WHITE);
        
        // Hook
        display.drawLine((int)px, (int)py, (int)px - cos(angle)*4 - sin(angle)*4, (int)py + sin(angle)*4 - cos(angle)*4, WHITE);
        display.drawLine((int)px, (int)py, (int)px + cos(angle)*4 - sin(angle)*4, (int)py - sin(angle)*4 - cos(angle)*4, WHITE);

        // Nuggets
        for(int i=0; i<4; i++) {
            if (nuggets[i].active) display.fillRect(nuggets[i].x-2, nuggets[i].y-2, 4, 4, WHITE);
        }
        if (grabbedIdx != -1) {
            display.fillRect((int)px-2, (int)py-2, 4, 4, WHITE);
        }

        display.setCursor(0,0); display.print(score);
        display.display();
        delay(15);
    }
};

REGISTER_GAME(GoldMinerGame)
