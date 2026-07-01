#pragma once

class SkateTrickGame : public ArcadeGame {
private:
    float py, pvy;
    int state; // 0=ramp, 1=air, 2=result
    int trick[3];
    int trickIdx;
    int score;

    void genTrick() {
        for(int i=0; i<3; i++) trick[i] = random(3);
        trickIdx = 0;
    }

public:
    const char* getCategory() override { return "Sports"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Skate Trick"; }

    void init() override {
        py = 50; pvy = -8.0; state = 0; score = 0;
    }

    void update() override {
        if (state == 0) { // Going up ramp
            py += pvy; pvy += 0.1;
            if (py < 20) { state = 1; genTrick(); delay(200); }
            
            display.clearDisplay();
            display.drawLine(10, 50, 40, 20, WHITE); // ramp
            display.fillRect(30, (int)py, 8, 8, WHITE); // skater
            display.display();
            delay(15);
        } else if (state == 1) { // In air
            py += pvy; pvy += 0.2;
            
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);
            
            int input = -1;
            if (cL) input = 0;
            if (cS) input = 1;
            if (cR) input = 2;

            if (input != -1) {
                if (input == trick[trickIdx]) {
                    trickIdx++;
                    if (trickIdx >= 3) { score += 100; state = 0; pvy = -8.0; delay(500); } // Landed it, go again
                    delay(150); // debounce
                } else {
                    state = 2; delay(500); // bailed
                }
            }

            if (py > 60) { state = 2; delay(500); } // hit ground without finishing

            display.clearDisplay();
            display.fillRect(60, (int)py, 8, 8, WHITE);
            
            display.setCursor(20, 10);
            for(int i=0; i<3; i++) {
                if (i < trickIdx) display.print("[OK] ");
                else {
                    if (trick[i] == 0) display.print("L ");
                    if (trick[i] == 1) display.print("S ");
                    if (trick[i] == 2) display.print("R ");
                }
            }
            display.display();
            delay(15);
        } else if (state == 2) {
            display.clearDisplay();
            display.setCursor(40, 20); display.print("BAILED!");
            display.setCursor(40, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(SkateTrickGame)
