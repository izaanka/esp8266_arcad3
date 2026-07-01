#pragma once

class Game2048 : public ArcadeGame {
private:
    int grid[4][4];
    int score = 0;
    int state = 0;
    int swipeDir = 0; // 0=Up, 1=Right, 2=Down, 3=Left
    bool lastSelect = true;

    void spawnTile() {
        int emptyCount = 0;
        for(int y=0; y<4; y++) for(int x=0; x<4; x++) if(grid[y][x] == 0) emptyCount++;
        if (emptyCount == 0) return;
        
        int pick = random(0, emptyCount);
        int c = 0;
        for(int y=0; y<4; y++) {
            for(int x=0; x<4; x++) {
                if(grid[y][x] == 0) {
                    if (c == pick) {
                        grid[y][x] = (random(10) < 9) ? 2 : 4;
                        return;
                    }
                    c++;
                }
            }
        }
    }

    bool slide() {
        bool moved = false;
        // Depending on swipeDir, we process rows/cols differently
        // Standard 2048 logic
        
        auto processLine = [&](int& a, int& b, int& c, int& d) {
            int line[4] = {a,b,c,d};
            int temp[4] = {0,0,0,0};
            int idx = 0;
            // Shift
            for(int i=0; i<4; i++) if (line[i] != 0) temp[idx++] = line[i];
            // Merge
            for(int i=0; i<3; i++) {
                if (temp[i] != 0 && temp[i] == temp[i+1]) {
                    temp[i] *= 2; score += temp[i]; temp[i+1] = 0;
                }
            }
            // Shift again
            int finalLine[4] = {0,0,0,0}; idx = 0;
            for(int i=0; i<4; i++) if (temp[i] != 0) finalLine[idx++] = temp[i];
            
            if (a!=finalLine[0] || b!=finalLine[1] || c!=finalLine[2] || d!=finalLine[3]) moved = true;
            
            a=finalLine[0]; b=finalLine[1]; c=finalLine[2]; d=finalLine[3];
        };

        if (swipeDir == 0) { // UP
            for(int x=0; x<4; x++) processLine(grid[0][x], grid[1][x], grid[2][x], grid[3][x]);
        } else if (swipeDir == 2) { // DOWN
            for(int x=0; x<4; x++) processLine(grid[3][x], grid[2][x], grid[1][x], grid[0][x]);
        } else if (swipeDir == 3) { // LEFT
            for(int y=0; y<4; y++) processLine(grid[y][0], grid[y][1], grid[y][2], grid[y][3]);
        } else if (swipeDir == 1) { // RIGHT
            for(int y=0; y<4; y++) processLine(grid[y][3], grid[y][2], grid[y][1], grid[y][0]);
        }
        
        return moved;
    }

public:
    const char* getCategory() override { return "Puzzle"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "2048"; }

    void init() override {
        for(int y=0; y<4; y++) for(int x=0; x<4; x++) grid[y][x] = 0;
        score = 0; state = 0; swipeDir = 0; lastSelect = true;
        spawnTile(); spawnTile();
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) { swipeDir = (swipeDir + 3) % 4; delay(150); }
            if (digitalRead(btnRight) == LOW) { swipeDir = (swipeDir + 1) % 4; delay(150); }
            
            bool currSelect = (digitalRead(btnSelect) == LOW);
            if (currSelect && !lastSelect) {
                if (slide()) {
                    spawnTile();
                    // Check game over (no empty spaces and no adjacent matches)
                    bool canMove = false;
                    for(int y=0; y<4; y++) for(int x=0; x<4; x++) {
                        if(grid[y][x]==0) canMove=true;
                        if(x<3 && grid[y][x]==grid[y][x+1]) canMove=true;
                        if(y<3 && grid[y][x]==grid[y+1][x]) canMove=true;
                    }
                    if (!canMove) state = 1;
                }
            }
            lastSelect = currSelect;
            
            display.clearDisplay();
            
            // Draw Grid
            int ox = 16, oy = 12;
            display.drawRect(ox-2, oy-2, 98, 52, WHITE);
            for(int y=0; y<4; y++) {
                for(int x=0; x<4; x++) {
                    if (grid[y][x] != 0) {
                        display.setCursor(ox + x*24, oy + y*12);
                        display.print(grid[y][x]);
                    }
                }
            }
            
            // Draw swipe direction indicator
            display.setCursor(0, 0); 
            if (swipeDir == 0) display.print("DIR: UP");
            if (swipeDir == 1) display.print("DIR: RIGHT");
            if (swipeDir == 2) display.print("DIR: DOWN");
            if (swipeDir == 3) display.print("DIR: LEFT");
            
            display.setCursor(70, 0); display.print("S:"); display.print(score);
            
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("GAME OVER");
            display.setCursor(30, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(Game2048)
