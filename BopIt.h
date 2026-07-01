#pragma once

class BopItGame : public ArcadeGame {
private:
    int score = 0;
    int state = 0;
    int targetAction = 0; // 0=left, 1=select, 2=right
    int timer = 100;
    int maxTimer = 100;
    bool btnL = true, btnS = true, btnR = true;

    void nextPrompt() {
        targetAction = random(0, 3);
        maxTimer = 100 - min(80, score * 2);
        timer = maxTimer;
    }

public:
    const char* getCategory() override { return "Arcade"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Bop It!"; }

    void init() override {
        score = 0; state = 0;
        btnL = true; btnS = true; btnR = true;
        nextPrompt();
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);
            
            int action = -1;
            if (cL && !btnL) action = 0;
            if (cS && !btnS) action = 1;
            if (cR && !btnR) action = 2;
            
            btnL = cL; btnS = cS; btnR = cR;
            
            if (action != -1) {
                if (action == targetAction) {
                    score++;
                    nextPrompt();
                } else {
                    state = 1; // Wrong button!
                    delay(500);
                }
            }
            
            timer -= 2;
            if (timer <= 0) {
                state = 1; // Timeout
                delay(500);
            }
            
            display.clearDisplay();
            
            display.setTextSize(2);
            display.setCursor(30, 20);
            if (targetAction == 0) display.print("LEFT!");
            else if (targetAction == 1) display.print("SELECT!");
            else display.print("RIGHT!");
            display.setTextSize(1);
            
            // Timer Bar
            display.drawRect(14, 50, 100, 6, WHITE);
            display.fillRect(14, 50, (timer * 100) / maxTimer, 6, WHITE);
            
            display.setCursor(0,0); display.print(score);
            
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("FAILED!");
            display.setCursor(35, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(BopItGame)
