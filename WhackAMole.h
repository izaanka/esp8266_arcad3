#pragma once

class WhackAMoleGame : public ArcadeGame {
private:
    struct Mole { int type; int timer; }; // type: 0=none, 1=mole, 2=bomb
    Mole holes[3];
    int score = 0;
    int state = 0;
    int hitTimer = 0;
    int hitHole = -1; // 0, 1, 2
    
    bool btnL = true, btnS = true, btnR = true;

public:
    const char* getCategory() override { return "Arcade"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Whack-A-Mole"; }

    void init() override {
        score = 0; state = 0; hitTimer = 0; hitHole = -1;
        for(int i=0; i<3; i++) { holes[i].type = 0; holes[i].timer = 0; }
        btnL = true; btnS = true; btnR = true;
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);
            
            int whack = -1;
            if (cL && !btnL) whack = 0;
            if (cS && !btnS) whack = 1;
            if (cR && !btnR) whack = 2;
            
            btnL = cL; btnS = cS; btnR = cR;
            
            if (whack != -1) {
                hitHole = whack;
                hitTimer = 10; // frames to show hammer
                if (holes[whack].type == 1) { // hit mole
                    score += 10;
                    holes[whack].type = 0;
                } else if (holes[whack].type == 2) { // hit bomb
                    state = 1; // game over
                    delay(500);
                }
            }
            
            if (hitTimer > 0) hitTimer--;
            
            // Mole Logic
            for(int i=0; i<3; i++) {
                if (holes[i].timer > 0) {
                    holes[i].timer--;
                    if (holes[i].timer == 0) holes[i].type = 0; // hide
                } else {
                    if (random(100) < 3) {
                        holes[i].type = (random(10) > 7) ? 2 : 1; // 20% bomb, 80% mole
                        holes[i].timer = random(30, 70) - min(30, score/10); // gets faster
                    }
                }
            }
            
            display.clearDisplay();
            
            display.setCursor(0,0); display.print("Score: "); display.print(score);
            
            int hX[3] = {24, 64, 104};
            for(int i=0; i<3; i++) {
                // Hole
                display.drawCircle(hX[i], 45, 12, WHITE);
                display.fillCircle(hX[i], 45, 10, BLACK);
                
                if (holes[i].type == 1) {
                    display.fillCircle(hX[i], 40, 8, WHITE);
                    display.drawPixel(hX[i]-3, 38, BLACK); // eyes
                    display.drawPixel(hX[i]+3, 38, BLACK);
                } else if (holes[i].type == 2) {
                    display.fillCircle(hX[i], 40, 8, WHITE);
                    display.drawLine(hX[i], 32, hX[i]+3, 28, WHITE); // fuse
                    // bomb icon
                    display.fillRect(hX[i]-2, 38, 4, 4, BLACK); 
                }
                
                if (hitTimer > 0 && hitHole == i) {
                    display.fillRect(hX[i] - 10, 20, 20, 10, WHITE); // Hammer head
                    display.drawLine(hX[i], 30, hX[i], 45, WHITE); // Hammer handle
                }
            }
            
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("BOOM!");
            display.setCursor(30, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(WhackAMoleGame)
