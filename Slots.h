#pragma once

class SlotsGame : public ArcadeGame {
private:
    int reels[3];
    float pos[3];
    float speeds[3];
    int state = 0; // 0=idle, 1=spinning, 2=stopping, 3=result
    int money = 100;
    int bet = 10;
    bool lastSelect = true;
    int stopTimer = 0;
    int currentStop = 0;

    void drawSymbol(int x, int y, int sym) {
        if (sym == 0) { display.drawCircle(x+8, y+8, 6, WHITE); } // Cherry
        else if (sym == 1) { display.fillRect(x+4, y+4, 8, 8, WHITE); } // Square
        else if (sym == 2) { display.drawTriangle(x+8, y+2, x+2, y+14, x+14, y+14, WHITE); } // Triangle
        else if (sym == 3) { display.drawLine(x+2, y+2, x+14, y+14, WHITE); display.drawLine(x+14, y+2, x+2, y+14, WHITE); } // X
        else if (sym == 4) { display.setCursor(x+5, y+4); display.print("$"); } // Jackpot
    }

public:
    const char* getName() override { return "Slot Machine"; }

    void init() override {
        reels[0]=0; reels[1]=0; reels[2]=0;
        pos[0]=0; pos[1]=0; pos[2]=0;
        state = 0; money = 100; lastSelect = true;
    }

    void update() override {
        bool cS = (digitalRead(btnSelect) == LOW);
        
        if (state == 0) {
            if (cS && !lastSelect && money >= bet) {
                money -= bet;
                speeds[0] = random(10, 20);
                speeds[1] = random(12, 22);
                speeds[2] = random(14, 24);
                state = 1;
                stopTimer = 100;
                currentStop = 0;
            }
            if (digitalRead(btnLeft) == LOW && !lastSelect) { returnToMenu(); delay(200); }
        } else if (state == 1) {
            for(int i=0; i<3; i++) {
                pos[i] += speeds[i];
                if (pos[i] > 160) pos[i] -= 160;
            }
            stopTimer--;
            if (stopTimer <= 0) {
                state = 2;
            }
        } else if (state == 2) {
            // Stopping sequence
            for(int i=0; i<3; i++) {
                if (i >= currentStop) {
                    pos[i] += speeds[i];
                    if (pos[i] > 160) pos[i] -= 160;
                }
            }
            // Snapping to grid for the current stopping reel
            if (speeds[currentStop] > 2) {
                speeds[currentStop] *= 0.9;
            } else {
                // Snap to nearest 32
                int target = ((int)(pos[currentStop] + 16) / 32) * 32;
                pos[currentStop] = target;
                reels[currentStop] = (target / 32) % 5;
                currentStop++;
                if (currentStop >= 3) {
                    state = 3; // Evaluate
                    delay(500);
                }
            }
        } else if (state == 3) {
            // Result
            if (reels[0] == reels[1] && reels[1] == reels[2]) {
                if (reels[0] == 4) money += bet * 10; // Jackpot
                else money += bet * 5; // Triple
            } else if (reels[0] == reels[1] || reels[1] == reels[2] || reels[0] == reels[2]) {
                money += bet * 2; // Pair
            }
            state = 0;
        }
        
        lastSelect = cS;
        
        display.clearDisplay();
        
        display.setCursor(0,0); display.print("CASH: $"); display.print(money);
        
        // Draw Machine
        display.drawRect(20, 16, 88, 32, WHITE);
        display.drawLine(49, 16, 49, 48, WHITE);
        display.drawLine(78, 16, 78, 48, WHITE);
        
        // Win Line
        display.drawLine(15, 32, 113, 32, WHITE);
        
        // Draw Reels (clipping manually by only drawing if inside box)
        for(int r=0; r<3; r++) {
            int cx = 25 + r*29;
            int offset = (int)pos[r] % 32; // 0 to 31
            
            for(int s=-1; s<=2; s++) {
                int symIdx = (int)(pos[r] / 32) - s;
                while(symIdx < 0) symIdx += 5;
                symIdx = symIdx % 5;
                
                int cy = 16 + (s*32) + offset;
                if (cy > 0 && cy < 48) {
                    drawSymbol(cx, cy - 8, symIdx);
                }
            }
        }
        
        if (state == 0) {
            display.setCursor(15, 54); display.print("D6: SPIN ($-10)");
            if (money < 10) { display.setCursor(15, 54); display.print("BANKRUPT! D5:EXIT"); }
        } else if (state == 3) {
            // Flash if win?
        }
        
        display.display();
        delay(15);
    }
};

REGISTER_GAME(SlotsGame)
