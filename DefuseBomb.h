#pragma once

class DefuseBombGame : public ArcadeGame {
private:
    int timer;
    int ruleIdx;
    int correctWire;
    int score;
    int state; // 0=play, 1=win, 2=loss
    bool lastL, lastS, lastR;

public:
    const char* getCategory() override { return "Puzzle"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Defuse Bomb"; }

    void nextBomb() {
        timer = 300; // 5 seconds (60fps * 5 = 300)
        ruleIdx = random(4);
        // Rules: 0=Left, 1=Mid, 2=Right, 3=Mid
        if (ruleIdx == 0) correctWire = 0;
        else if (ruleIdx == 1) correctWire = 1;
        else if (ruleIdx == 2) correctWire = 2;
        else correctWire = 1;
        
        lastL = true; lastS = true; lastR = true;
    }

    void init() override {
        score = 0; state = 0;
        nextBomb();
    }

    void update() override {
        if (state == 0) {
            timer--;
            if (timer <= 0) { state = 2; delay(500); }

            bool cL = (digitalRead(btnLeft) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);

            int choice = -1;
            if (cL && !lastL) choice = 0;
            if (cS && !lastS) choice = 1;
            if (cR && !lastR) choice = 2;

            lastL = cL; lastS = cS; lastR = cR;

            if (choice != -1) {
                if (choice == correctWire) {
                    score++; nextBomb();
                } else {
                    state = 2; delay(500);
                }
            }

            display.clearDisplay();
            
            display.setCursor(50, 0); 
            display.print(timer / 60.0, 1);

            display.setCursor(10, 15);
            if (ruleIdx == 0) display.print("RULE: Cut Left");
            else if (ruleIdx == 1) display.print("RULE: Cut Middle");
            else if (ruleIdx == 2) display.print("RULE: Cut Right");
            else display.print("RULE: Not L, Not R");

            display.drawLine(32, 40, 32, 64, WHITE);
            display.drawLine(64, 40, 64, 64, WHITE);
            display.drawLine(96, 40, 96, 64, WHITE);

            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(40, 20); display.print("BOOM!");
            display.setCursor(40, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(DefuseBombGame)
