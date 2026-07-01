#pragma once

class StackerGame : public ArcadeGame {
private:
    byte grid[10];
    int currRow = 0;
    int currCol = 0;
    int currWidth = 3;
    int dir = 1;
    
    unsigned long lastMove = 0;
    int speed = 150;
    
    int state = 400; // 400=Playing, 401=Win, 402=Lose
    bool lastSelect = true;

public:
    const char* getCategory() override { return "Puzzle"; }
    const char* getName() override { return "Stacker"; }

    void init() override {
        for(int i=0; i<10; i++) grid[i] = 0;
        currRow = 0;
        currCol = 0;
        currWidth = 3;
        dir = 1;
        speed = 150;
        state = 400;
        lastSelect = true;
        lastMove = millis();
    }

    void update() override {
        if (state == 400) {
            bool currSelect = digitalRead(btnSelect);
            bool pressed = (currSelect == LOW && lastSelect == HIGH);
            lastSelect = currSelect;

            if (millis() - lastMove > speed) {
                currCol += dir;
                if (currCol <= 0) {
                    currCol = 0;
                    dir = 1;
                }
                if (currCol + currWidth >= 7) {
                    currCol = 7 - currWidth;
                    dir = -1;
                }
                lastMove = millis();
            }

            if (pressed) {
                // Calculate bits for current block
                byte blockBits = 0;
                for(int i=0; i<currWidth; i++) {
                    blockBits |= (1 << (currCol + i));
                }

                if (currRow == 0) {
                    grid[currRow] = blockBits;
                    currRow++;
                    speed -= 10;
                } else {
                    byte prevBits = grid[currRow - 1];
                    byte matched = blockBits & prevBits;
                    
                    if (matched == 0) {
                        state = 402; // Lose
                    } else {
                        grid[currRow] = matched;
                        
                        // Count how many blocks matched
                        int newWidth = 0;
                        int firstCol = -1;
                        for(int i=0; i<7; i++) {
                            if (matched & (1 << i)) {
                                newWidth++;
                                if (firstCol == -1) firstCol = i;
                            }
                        }
                        
                        currWidth = newWidth;
                        currRow++;
                        
                        // Start next row
                        if (currRow >= 10) {
                            state = 401; // Win
                        } else {
                            // Shrink width at certain milestones
                            if (currRow == 4 && currWidth > 2) currWidth = 2;
                            if (currRow == 7 && currWidth > 1) currWidth = 1;
                            
                            currCol = 0;
                            dir = 1;
                            speed = max(40, speed - 12);
                        }
                    }
                }
            }

            display.clearDisplay();
            
            // Draw grid bounds
            display.drawRect(6, 2, 116, 62, WHITE);
            
            // Draw placed blocks
            for(int r=0; r<10; r++) {
                for(int c=0; c<7; c++) {
                    if (grid[r] & (1 << c)) {
                        int px = 8 + c * 16;
                        int py = 57 - r * 6;
                        display.fillRect(px, py, 14, 4, WHITE);
                    }
                }
            }
            
            // Draw moving block
            if (state == 400) {
                for(int i=0; i<currWidth; i++) {
                    int px = 8 + (currCol + i) * 16;
                    int py = 57 - currRow * 6;
                    display.fillRect(px, py, 14, 4, WHITE);
                }
            }
            
            display.display();
            delay(15);
        }
        else if (state == 401 || state == 402) {
            display.clearDisplay();
            display.setCursor(20, 20);
            if (state == 401) display.print("MAJOR PRIZE!");
            else display.print("GAME OVER");
            
            display.setCursor(20, 35);
            display.print("Rows: "); display.print(currRow);
            
            display.setCursor(20, 50); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(StackerGame)
