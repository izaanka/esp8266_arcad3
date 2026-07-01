#pragma once

class BowlingGame : public ArcadeGame {
private:
    float bX, bY, bVX, bVY;
    int state = 0; // 0=pos, 1=angle, 2=power, 3=roll, 4=result
    
    struct Pin { float x, y; bool active; };
    Pin pins[6];
    
    float setPos = 64;
    float setAng = 0;
    float setPow = 0;
    int dir = 1;
    
    int score = 0;
    bool lastSelect = true;

    void resetPins() {
        pins[0].x = 64; pins[0].y = 15;
        pins[1].x = 59; pins[1].y = 10;
        pins[2].x = 69; pins[2].y = 10;
        pins[3].x = 54; pins[3].y = 5;
        pins[4].x = 64; pins[4].y = 5;
        pins[5].x = 74; pins[5].y = 5;
        for(int i=0; i<6; i++) pins[i].active = true;
    }

public:
    const char* getCategory() override { return "Sports"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Bowling"; }

    void init() override {
        score = 0; state = 0; setPos = 64; dir = 1; lastSelect = true;
        resetPins();
    }

    void update() override {
        bool cS = (digitalRead(btnSelect) == LOW);
        
        if (state == 0) {
            setPos += dir * 1.5;
            if (setPos > 96 || setPos < 32) dir = -dir;
            if (cS && !lastSelect) { state = 1; setAng = 0; dir = 1; }
        } else if (state == 1) {
            setAng += dir * 0.05;
            if (setAng > 1.0 || setAng < -1.0) dir = -dir; // angle sweep
            if (cS && !lastSelect) { state = 2; setPow = 0.5; dir = 1; }
        } else if (state == 2) {
            setPow += dir * 0.2;
            if (setPow > 5.0 || setPow < 0.5) dir = -dir;
            if (cS && !lastSelect) { 
                state = 3; 
                bX = setPos; bY = 56;
                bVX = sin(setAng) * setPow;
                bVY = -cos(setAng) * setPow;
            }
        } else if (state == 3) {
            bX += bVX; bY += bVY;
            
            // Wall bounce
            if (bX < 32) { bX = 32; bVX = -bVX * 0.5; }
            if (bX > 96) { bX = 96; bVX = -bVX * 0.5; }
            
            // Pin collision
            for(int i=0; i<6; i++) {
                if (pins[i].active) {
                    float dx = bX - pins[i].x;
                    float dy = bY - pins[i].y;
                    if (dx*dx + dy*dy < 16) {
                        pins[i].active = false;
                        score++;
                        // Deflect ball slightly
                        bVX += dx * 0.1;
                        bVY += dy * 0.1;
                    }
                }
            }
            
            if (bY < -5) {
                state = 4;
                delay(500);
            }
        } else if (state == 4) {
            if (cS && !lastSelect) {
                resetPins();
                state = 0;
            }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
        }
        lastSelect = cS;
        
        display.clearDisplay();
        
        // Draw Lane
        display.drawLine(30, 0, 30, 64, WHITE);
        display.drawLine(98, 0, 98, 64, WHITE);
        
        // Draw Pins
        for(int i=0; i<6; i++) {
            if (pins[i].active) display.fillCircle((int)pins[i].x, (int)pins[i].y, 2, WHITE);
        }
        
        if (state == 0) {
            display.drawCircle((int)setPos, 56, 3, WHITE); // Pos indicator
        } else if (state == 1) {
            display.fillCircle((int)setPos, 56, 3, WHITE);
            display.drawLine((int)setPos, 56, (int)setPos + sin(setAng)*15, 56 - cos(setAng)*15, WHITE);
        } else if (state == 2) {
            display.fillCircle((int)setPos, 56, 3, WHITE);
            display.drawLine((int)setPos, 56, (int)setPos + sin(setAng)*15, 56 - cos(setAng)*15, WHITE);
            // Power bar
            display.drawRect(5, 10, 6, 40, WHITE);
            display.fillRect(5, 50 - (int)(setPow * 8), 6, (int)(setPow * 8), WHITE);
        } else if (state == 3) {
            display.fillCircle((int)bX, (int)bY, 3, WHITE);
        } else if (state == 4) {
            display.setCursor(35, 30); display.print("SCORE: "); display.print(score);
        }
        
        display.setCursor(110, 0); display.print(score);
        display.display();
        delay(15);
    }
};

REGISTER_GAME(BowlingGame)
