#pragma once

class TimberGame : public ArcadeGame {
private:
    int score = 0;
    int state = 0;
    int playerSide = 0; // 0=left, 1=right
    int branches[6]; // 0=none, 1=left, 2=right
    
    bool btnL = true, btnR = true;
    
    int timer = 100;

    void addBranch() {
        // Shift down
        for(int i=0; i<5; i++) branches[i] = branches[i+1];
        // New top branch
        if (branches[4] != 0) branches[5] = 0; // never two consecutive branches
        else branches[5] = (random(10) > 4) ? ((random(2)==0)?1:2) : 0;
    }

public:
    const char* getCategory() override { return "Arcade"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Timber"; }

    void init() override {
        score = 0; state = 0; playerSide = 0; timer = 100;
        btnL = true; btnR = true;
        for(int i=0; i<6; i++) branches[i] = 0;
        branches[0] = 0; branches[1] = 0;
        addBranch(); addBranch(); addBranch(); addBranch();
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);
            
            bool chop = false;
            if (cL && !btnL) { playerSide = 0; chop = true; }
            if (cR && !btnR) { playerSide = 1; chop = true; }
            
            btnL = cL; btnR = cR;
            
            if (chop) {
                addBranch();
                score++;
                timer += 5;
                if (timer > 100) timer = 100;
                
                // Check death
                if (playerSide == 0 && branches[0] == 1) state = 1;
                if (playerSide == 1 && branches[0] == 2) state = 1;
                if (state == 1) delay(500);
            }
            
            timer -= 1;
            if (timer <= 0) {
                state = 1; // Time up
                delay(500);
            }
            
            if (state == 0) {
                display.clearDisplay();
                
                // Tree Trunk
                display.fillRect(54, 0, 20, 64, WHITE);
                
                // Branches
                for(int i=0; i<6; i++) {
                    int y = 50 - (i*10);
                    if (branches[i] == 1) display.fillRect(34, y, 20, 8, WHITE);
                    if (branches[i] == 2) display.fillRect(74, y, 20, 8, WHITE);
                }
                
                // Player
                int px = (playerSide == 0) ? 38 : 78;
                display.fillRect(px, 50, 12, 14, WHITE);
                // Axe
                if (chop) {
                    if (playerSide == 0) {
                        display.drawLine(px+12, 57, px+18, 57, WHITE);
                        display.fillRect(px+16, 55, 4, 5, BLACK); display.drawRect(px+16, 55, 4, 5, WHITE);
                    } else {
                        display.drawLine(px, 57, px-6, 57, WHITE);
                        display.fillRect(px-8, 55, 4, 5, BLACK); display.drawRect(px-8, 55, 4, 5, WHITE);
                    }
                }
                
                // Timer
                display.drawRect(4, 4, 6, 50, WHITE);
                display.fillRect(4, 54 - (timer/2), 6, (timer/2), WHITE);
                
                display.setCursor(0, 56); display.print(score);
                
                display.display();
                delay(15);
            }
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("SQUASHED!");
            display.setCursor(35, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(TimberGame)
