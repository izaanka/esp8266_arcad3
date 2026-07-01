#pragma once

class CraneGame : public ArcadeGame {
private:
    float cx, cy;
    int state; // 0=move right, 1=move down, 2=grab, 3=win/loss
    int px, py;
    bool caught;
    bool lastS, lastR;

public:
    const char* getCategory() override { return "Action"; }
    const char* getName() override { return "Crane Game"; }

    void init() override {
        cx = 10; cy = 10; state = 0; caught = false;
        px = random(40, 110); py = 50;
        lastS = true; lastR = true;
    }

    void update() override {
        bool cS = (digitalRead(btnSelect) == LOW);
        bool cR = (digitalRead(btnRight) == LOW);

        if (state == 0) {
            if (cR) cx += 1.0;
            if (!cR && !lastR && cx > 10) state = 1; // stopped moving right
            if (cx > 120) state = 1;
        } else if (state == 1) {
            if (cS) cy += 1.0;
            if (!cS && !lastS && cy > 10) state = 2; // stopped moving down
            if (cy > 50) state = 2;
        } else if (state == 2) {
            if (abs(cx - px) < 8 && abs(cy - py) < 8) caught = true;
            state = 3; delay(500);
        } else if (state == 3) {
            display.clearDisplay();
            display.setCursor(30, 20); 
            if (caught) display.print("YOU WON!"); else display.print("MISSED!");
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
            return;
        }

        lastR = cR; lastS = cS;

        display.clearDisplay();
        
        display.drawLine(0, 10, 128, 10, WHITE); // rail
        
        // Prize
        if (!caught || state != 2) display.fillRect(px-4, py, 8, 8, WHITE);
        else display.fillRect((int)cx-4, (int)cy, 8, 8, WHITE); // grasped
        
        // Crane
        display.drawLine((int)cx, 10, (int)cx, (int)cy, WHITE);
        if (state == 2) { // closed
            display.drawLine((int)cx, (int)cy, (int)cx-3, (int)cy+6, WHITE);
            display.drawLine((int)cx, (int)cy, (int)cx+3, (int)cy+6, WHITE);
        } else { // open
            display.drawLine((int)cx, (int)cy, (int)cx-6, (int)cy+6, WHITE);
            display.drawLine((int)cx, (int)cy, (int)cx+6, (int)cy+6, WHITE);
        }
        
        display.display();
        delay(15);
    }
};

REGISTER_GAME(CraneGame)
