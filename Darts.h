#pragma once

class DartsGame : public ArcadeGame {
private:
    float cx, cy;
    int state; // 0=x move, 1=y move, 2=result
    int dir;
    int score;
    int throws;

public:
    const char* getCategory() override { return "Sports"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Darts"; }

    void init() override {
        cx = 10; cy = 32;
        state = 0; dir = 1;
        score = 0; throws = 0;
    }

    void update() override {
        if (state == 0) {
            cx += dir * 2.5;
            if (cx > 118 || cx < 10) dir = -dir;
            if (digitalRead(btnSelect) == LOW) { state = 1; dir = 1; delay(200); }
        } else if (state == 1) {
            cy += dir * 2.5;
            if (cy > 54 || cy < 10) dir = -dir;
            if (digitalRead(btnSelect) == LOW) { 
                state = 2; 
                float dx = cx - 64; float dy = cy - 32;
                float dist = sqrt(dx*dx + dy*dy);
                if (dist < 4) score += 50;
                else if (dist < 12) score += 20;
                else if (dist < 24) score += 10;
                throws++;
                delay(500); 
            }
        } else if (state == 2) {
            if (throws >= 3) {
                display.clearDisplay();
                display.setCursor(30, 20); display.print("GAME OVER");
                display.setCursor(30, 35); display.print("Score: "); display.print(score);
                if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
                if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
                display.display();
                return;
            } else {
                cx = 10; cy = 32; state = 0; dir = 1;
            }
        }

        display.clearDisplay();
        
        // Target
        display.drawCircle(64, 32, 24, WHITE);
        display.drawCircle(64, 32, 12, WHITE);
        display.fillCircle(64, 32, 4, WHITE);

        // Crosshair
        if (state == 0) {
            display.drawLine(cx, 0, cx, 64, WHITE);
        } else if (state == 1) {
            display.drawLine(cx, 0, cx, 64, WHITE);
            display.drawLine(0, cy, 128, cy, WHITE);
        } else if (state == 2) {
            display.drawLine(cx-4, cy, cx+4, cy, WHITE);
            display.drawLine(cx, cy-4, cx, cy+4, WHITE);
        }

        display.setCursor(0, 0); display.print(score);
        display.setCursor(110, 0); display.print(3-throws);
        display.display();
        delay(15);
    }
};

REGISTER_GAME(DartsGame)
