#pragma once

class MemoryGame : public ArcadeGame {
private:
    int grid[16];
    bool revealed[16];
    bool matched[16];
    
    int cursor = 0;
    int state = 0; // 0=select1, 1=select2, 2=wait, 3=win
    int pick1 = -1;
    int pick2 = -1;
    int waitTimer = 0;
    int moves = 0;
    
    bool lastSelect = true;

public:
    const char* getName() override { return "Memory Match"; }

    void init() override {
        cursor = 0; state = 0; pick1 = -1; pick2 = -1; moves = 0; lastSelect = true;
        
        int pairs[16] = {0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7};
        // Shuffle
        for(int i=0; i<16; i++) {
            int r = random(0, 16);
            int temp = pairs[i];
            pairs[i] = pairs[r];
            pairs[r] = temp;
        }
        
        for(int i=0; i<16; i++) {
            grid[i] = pairs[i];
            revealed[i] = false;
            matched[i] = false;
        }
    }

    void update() override {
        if (state == 0 || state == 1) {
            if (digitalRead(btnLeft) == LOW) { cursor = (cursor > 0) ? cursor - 1 : 15; delay(150); }
            if (digitalRead(btnRight) == LOW) { cursor = (cursor < 15) ? cursor + 1 : 0; delay(150); }
            
            bool currSelect = (digitalRead(btnSelect) == LOW);
            if (currSelect && !lastSelect && !matched[cursor] && !revealed[cursor]) {
                revealed[cursor] = true;
                
                if (state == 0) {
                    pick1 = cursor;
                    state = 1;
                } else if (state == 1) {
                    pick2 = cursor;
                    moves++;
                    if (grid[pick1] == grid[pick2]) {
                        matched[pick1] = true;
                        matched[pick2] = true;
                        state = 0;
                        
                        // check win
                        bool win = true;
                        for(int i=0; i<16; i++) if (!matched[i]) win = false;
                        if (win) state = 3;
                    } else {
                        state = 2;
                        waitTimer = 60; // frames to wait
                    }
                }
            }
            lastSelect = currSelect;
            
        } else if (state == 2) { // waiting before flipping back
            waitTimer--;
            if (waitTimer <= 0) {
                revealed[pick1] = false;
                revealed[pick2] = false;
                state = 0;
            }
        }
        
        if (state != 3) {
            display.clearDisplay();
            
            // Draw grid
            int ox = 16, oy = 8;
            for(int i=0; i<16; i++) {
                int x = ox + (i % 4) * 25;
                int y = oy + (i / 4) * 13;
                
                if (matched[i] || revealed[i]) {
                    display.drawRect(x, y, 22, 11, WHITE);
                    display.setCursor(x + 8, y + 2);
                    display.print(grid[i]);
                } else {
                    display.fillRect(x, y, 22, 11, WHITE);
                }
                
                if (i == cursor && state != 2) {
                    display.drawRect(x-2, y-2, 26, 15, WHITE);
                }
            }
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("YOU WIN!");
            display.setCursor(30, 35); display.print("Moves: "); display.print(moves);
            display.setCursor(20, 50); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(MemoryGame)
