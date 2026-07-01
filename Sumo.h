#pragma once

class SumoGame : public ArcadeGame {
private:
    float pos;
    int state;
    int lastBtn;
    int score;

public:
    const char* getCategory() override { return "Sports"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Sumo"; }

    void init() override {
        pos = 64; state = 0; lastBtn = -1; score = 0;
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);

            // AI pushes left
            pos -= random(1, 4) * 0.4;

            // Player pushes right by mashing
            if (cL && lastBtn != 0) { pos += 1.5; lastBtn = 0; }
            if (cR && lastBtn != 1) { pos += 1.5; lastBtn = 1; }
            if (!cL && !cR) lastBtn = -1;

            if (pos < 20) { state = 1; score = 0; delay(500); } // AI won
            if (pos > 108) { state = 1; score = 1; delay(500); } // Player won
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); 
            if (score == 1) display.print("YOU WON!"); else display.print("AI WON!");
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
            return;
        }

        display.clearDisplay();
        
        // Ring
        display.drawLine(20, 50, 108, 50, WHITE);
        display.drawLine(20, 48, 20, 52, WHITE);
        display.drawLine(108, 48, 108, 52, WHITE);

        // Player & AI pushing at 'pos'
        display.fillRect((int)pos - 8, 40, 8, 10, WHITE); // AI
        display.drawRect((int)pos, 40, 8, 10, WHITE); // Player
        
        display.setCursor(0,0); display.print("Mash L/R!");
        display.display();
        delay(15);
    }
};

REGISTER_GAME(SumoGame)
