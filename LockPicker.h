#pragma once

class LockPickerGame : public ArcadeGame {
private:
    float angle;
    int target;
    int pins;
    int state; // 0=playing, 1=win/loss
    int score;
    bool lastSelect;
    int dir;

public:
    const char* getCategory() override { return "Puzzle"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Lock Picker"; }

    void init() override {
        angle = 0; pins = 5; state = 0; score = 0; dir = 1;
        target = random(30, 330);
        lastSelect = true;
    }

    void update() override {
        bool cS = (digitalRead(btnSelect) == LOW);

        if (state == 0) {
            angle += dir * 4.0;
            if (angle > 360) angle -= 360;
            if (angle < 0) angle += 360;

            if (cS && !lastSelect) {
                // Check if aligned
                float diff = abs(angle - target);
                if (diff > 180) diff = 360 - diff;
                
                if (diff < 15) { // Hit
                    score += 10;
                    pins--;
                    if (pins <= 0) { state = 1; delay(500); }
                    else {
                        dir = -dir; // reverse
                        target = random(30, 330);
                        delay(200);
                    }
                } else { // Miss
                    state = 1; delay(500);
                }
            }
        } else if (state == 1) {
            display.clearDisplay();
            display.setCursor(20, 20); 
            if (pins <= 0) display.print("UNLOCKED!"); else display.print("SNAP! BROKE LOCK");
            display.setCursor(20, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
            return;
        }
        
        lastSelect = cS;

        display.clearDisplay();
        
        // Draw Lock ring
        display.drawCircle(64, 32, 20, WHITE);
        
        // Draw Target Notch
        float tx = 64 + sin(target * PI / 180.0) * 18;
        float ty = 32 - cos(target * PI / 180.0) * 18;
        display.fillCircle((int)tx, (int)ty, 3, WHITE);

        // Draw Player Pick
        float px = 64 + sin(angle * PI / 180.0) * 20;
        float py = 32 - cos(angle * PI / 180.0) * 20;
        display.drawLine(64, 32, (int)px, (int)py, WHITE);
        
        // Draw Pins remaining
        for(int i=0; i<pins; i++) {
            display.fillRect(10, 50 - i*6, 4, 4, WHITE);
        }

        display.setCursor(100, 0); display.print(score);
        display.display();
        delay(15);
    }
};

REGISTER_GAME(LockPickerGame)
