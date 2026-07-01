#pragma once

class Connect4Game : public ArcadeGame {
private:
    int grid[6][7]; // 0=empty, 1=player, 2=ai
    int cursor = 3;
    int state = 0; // 0=player, 1=drop, 2=ai, 3=win/loss
    int winner = 0;
    
    int dropCol = 0;
    float dropY = 0;
    int dropTargetY = 0;
    int dropPlayer = 0;
    
    bool lastSelect = true;

    bool checkWin(int p) {
        for(int r=0; r<6; r++) {
            for(int c=0; c<7; c++) {
                if(grid[r][c] == p) {
                    if (c<=3 && grid[r][c+1]==p && grid[r][c+2]==p && grid[r][c+3]==p) return true;
                    if (r<=2 && grid[r+1][c]==p && grid[r+2][c]==p && grid[r+3][c]==p) return true;
                    if (c<=3 && r<=2 && grid[r+1][c+1]==p && grid[r+2][c+2]==p && grid[r+3][c+3]==p) return true;
                    if (c>=3 && r<=2 && grid[r+1][c-1]==p && grid[r+2][c-2]==p && grid[r+3][c-3]==p) return true;
                }
            }
        }
        return false;
    }

    void dropToken(int c, int p) {
        dropCol = c;
        dropPlayer = p;
        dropY = -10;
        int targetR = 5;
        while(targetR >= 0 && grid[targetR][c] != 0) targetR--;
        dropTargetY = targetR * 8 + 14;
        grid[targetR][c] = p; // temporarily place it for logic, we will animate it
        state = 1;
    }

public:
    const char* getCategory() override { return "Puzzle"; }
    const char* getName() override { return "Connect 4"; }

    void init() override {
        for(int r=0; r<6; r++) for(int c=0; c<7; c++) grid[r][c] = 0;
        cursor = 3; state = 0; winner = 0; lastSelect = true;
    }

    void update() override {
        if (state == 0) { // Player Turn
            if (digitalRead(btnLeft) == LOW) { cursor = (cursor > 0) ? cursor - 1 : 6; delay(150); }
            if (digitalRead(btnRight) == LOW) { cursor = (cursor < 6) ? cursor + 1 : 0; delay(150); }
            
            bool currSelect = (digitalRead(btnSelect) == LOW);
            if (currSelect && !lastSelect) {
                if (grid[0][cursor] == 0) {
                    dropToken(cursor, 1);
                }
            }
            lastSelect = currSelect;
            
        } else if (state == 1) { // Animate Drop
            dropY += 4.0;
            if (dropY >= dropTargetY) {
                if (checkWin(dropPlayer)) {
                    winner = dropPlayer;
                    state = 3;
                    delay(500);
                } else {
                    bool full = true;
                    for(int c=0; c<7; c++) if (grid[0][c]==0) full = false;
                    if (full) { winner = 0; state = 3; delay(500); }
                    else {
                        if (dropPlayer == 1) {
                            state = 2; // AI turn
                            delay(500);
                        } else {
                            state = 0; // Player turn
                        }
                    }
                }
            }
        } else if (state == 2) { // AI Turn
            int aiCol = random(0, 7);
            while(grid[0][aiCol] != 0) aiCol = random(0, 7);
            
            // Simple block (look ahead 1)
            for(int c=0; c<7; c++) {
                if (grid[0][c] == 0) {
                    int r = 5; while(r>=0 && grid[r][c]!=0) r--;
                    grid[r][c] = 1;
                    if (checkWin(1)) aiCol = c;
                    grid[r][c] = 0;
                }
            }
            
            dropToken(aiCol, 2);
        }
        
        display.clearDisplay();
        
        // Draw Grid
        int ox = (128 - (7*10))/2;
        int oy = 12;
        
        display.drawRect(ox-2, oy-2, 7*10 + 4, 6*9 + 4, WHITE);
        
        for(int r=0; r<6; r++) {
            for(int c=0; c<7; c++) {
                int cx = ox + c*10 + 4;
                int cy = oy + r*9 + 4;
                
                if (state == 1 && c == dropCol && dropTargetY == cy) {
                    // It's dropping, don't draw it yet in the grid
                } else {
                    if (grid[r][c] == 1) display.fillCircle(cx, cy, 3, WHITE);
                    else if (grid[r][c] == 2) display.drawCircle(cx, cy, 3, WHITE);
                    else display.drawPixel(cx, cy, WHITE);
                }
            }
        }
        
        // Draw Falling token
        if (state == 1) {
            int cx = ox + dropCol*10 + 4;
            if (dropPlayer == 1) display.fillCircle(cx, (int)dropY + 4, 3, WHITE);
            else display.drawCircle(cx, (int)dropY + 4, 3, WHITE);
        }
        
        // Draw Cursor
        if (state == 0) {
            int cx = ox + cursor*10;
            display.drawLine(cx, 2, cx+8, 2, WHITE);
            display.fillTriangle(cx+4, 8, cx+2, 4, cx+6, 4, WHITE);
        }
        
        if (state == 3) {
            display.fillRect(20, 20, 88, 24, BLACK);
            display.drawRect(20, 20, 88, 24, WHITE);
            display.setCursor(25, 28);
            if (winner == 1) display.print("YOU WIN!");
            else if (winner == 2) display.print("AI WINS!");
            else display.print("DRAW!");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
        }
        
        display.display();
        delay(15);
    }
};

REGISTER_GAME(Connect4Game)
