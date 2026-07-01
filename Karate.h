#pragma once

class KarateGame : public ArcadeGame {
private:
    int pHP, aiHP;
    int pState, aiState; // 0=idle, 1=block, 2=punch, 3=kick, 4=hit
    int timer;
    int state;
    bool lastL, lastS, lastR;

public:
    const char* getCategory() override { return "Action"; }
    const char* getName() override { return "Karate"; }
    int getScore() override { return aiHP == 0 ? 1 : 0; }

    void init() override {
        pHP = 5; aiHP = 5;
        pState = 0; aiState = 0;
        timer = 50; state = 0;
        lastL = true; lastS = true; lastR = true;
    }

    void drawFighter(int x, int s, bool flipped) {
        display.drawLine(x, 40, x, 50, WHITE); // Body
        display.drawCircle(x, 35, 5, WHITE); // Head
        
        if (s == 0 || s == 4) { // Idle / hit
            display.drawLine(x, 42, x + (flipped?-5:5), 48, WHITE);
            display.drawLine(x, 50, x - (flipped?-5:5), 60, WHITE);
            display.drawLine(x, 50, x + (flipped?-5:5), 60, WHITE);
        } else if (s == 1) { // Block
            display.drawLine(x, 42, x + (flipped?-3:3), 35, WHITE); // arm up
            display.drawLine(x, 50, x - (flipped?-5:5), 60, WHITE);
            display.drawLine(x, 50, x + (flipped?-5:5), 60, WHITE);
        } else if (s == 2) { // Punch
            display.drawLine(x, 42, x + (flipped?-12:12), 42, WHITE); // arm out
            display.drawLine(x, 50, x - (flipped?-5:5), 60, WHITE);
            display.drawLine(x, 50, x + (flipped?-5:5), 60, WHITE);
        } else if (s == 3) { // Kick
            display.drawLine(x, 42, x + (flipped?-5:5), 48, WHITE);
            display.drawLine(x, 50, x - (flipped?-5:5), 60, WHITE);
            display.drawLine(x, 50, x + (flipped?-12:12), 48, WHITE); // leg out
        }
    }

    void update() override {
        if (state == 0) {
            timer--;
            
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);

            if (pState == 0) {
                if (cL && !lastL) pState = 1; // Block
                if (cS && !lastS) pState = 2; // Punch
                if (cR && !lastR) pState = 3; // Kick
            }

            // AI Logic (random moves)
            if (aiState == 0 && timer % 20 == 0 && random(100) < 40) {
                aiState = random(1, 4);
            }

            // Resolve attacks
            if (timer <= 0 || (pState != 0 && aiState != 0)) {
                if (pState == 2 && aiState == 3) { aiHP--; aiState = 4; } // Punch beats kick
                if (pState == 3 && aiState == 1) { aiHP--; aiState = 4; } // Kick breaks block
                if (pState == 1 && aiState == 2) { aiHP--; aiState = 4; } // Block stops punch
                
                if (aiState == 2 && pState == 3) { pHP--; pState = 4; }
                if (aiState == 3 && pState == 1) { pHP--; pState = 4; }
                if (aiState == 1 && pState == 2) { pHP--; pState = 4; }
                
                timer = 30; // hold animation
                if (pHP <= 0 || aiHP <= 0) state = 1;
            }
            
            if (timer == 10 && pState != 0 && aiState != 0) {
                pState = 0; aiState = 0; timer = 50;
            }

            lastL = cL; lastS = cS; lastR = cR;

            display.clearDisplay();
            display.setCursor(0, 0); display.print("HP: "); display.print(pHP);
            display.setCursor(90, 0); display.print("HP: "); display.print(aiHP);
            
            drawFighter(40, pState, false);
            drawFighter(88, aiState, true);
            
            if (pState == 4) { display.setCursor(20, 20); display.print("BAM!"); }
            if (aiState == 4) { display.setCursor(90, 20); display.print("BAM!"); }
            
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); 
            if (pHP > 0) display.print("YOU WON!");
            else display.print("YOU LOST!");
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(KarateGame)
