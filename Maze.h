#pragma once

class MazeGame : public ArcadeGame {
private:
    int map[10][16] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,1,0,0,0,0,0,1,0,0,0,3,1},
        {1,0,1,0,1,0,1,1,1,0,1,0,1,1,1,1},
        {1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1},
        {1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1},
        {1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,1},
        {1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,1},
        {1,4,1,0,0,0,1,0,0,0,0,0,1,1,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };
    
    int pX, pY;
    int dir = 0; // 0=up, 1=right, 2=down, 3=left
    int state = 0;
    int moves = 0;
    bool lastSelect = true;

public:
    const char* getCategory() override { return "Puzzle"; }
    const char* getName() override { return "Maze Runner"; }

    void init() override {
        // find player and target
        for(int y=0; y<10; y++) {
            for(int x=0; x<16; x++) {
                if (map[y][x] == 4) { pX = x; pY = y; }
            }
        }
        dir = 0; state = 0; moves = 0; lastSelect = true;
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) { dir = (dir + 3) % 4; delay(150); }
            if (digitalRead(btnRight) == LOW) { dir = (dir + 1) % 4; delay(150); }
            
            bool currSelect = (digitalRead(btnSelect) == LOW);
            if (currSelect && !lastSelect) {
                int nx = pX, ny = pY;
                if (dir == 0) ny--;
                else if (dir == 1) nx++;
                else if (dir == 2) ny++;
                else nx--;
                
                if (map[ny][nx] != 1) {
                    pX = nx; pY = ny;
                    moves++;
                    if (map[pY][pX] == 3) {
                        state = 1; // Win!
                        delay(500);
                    }
                }
            }
            lastSelect = currSelect;
            
            display.clearDisplay();
            
            // Draw Map (cell size 8x6)
            for(int y=0; y<10; y++) {
                for(int x=0; x<16; x++) {
                    int cx = x * 8;
                    int cy = y * 6;
                    if (map[y][x] == 1) display.fillRect(cx, cy, 8, 6, WHITE);
                    if (map[y][x] == 3) {
                        display.drawCircle(cx+4, cy+3, 2, WHITE); // Target
                    }
                }
            }
            
            // Draw Player
            int cx = pX * 8; int cy = pY * 6;
            if (dir == 0) display.fillTriangle(cx+4, cy, cx+2, cy+5, cx+6, cy+5, WHITE); // UP
            if (dir == 1) display.fillTriangle(cx+7, cy+3, cx+2, cy+1, cx+2, cy+5, WHITE); // RIGHT
            if (dir == 2) display.fillTriangle(cx+4, cy+6, cx+2, cy+1, cx+6, cy+1, WHITE); // DOWN
            if (dir == 3) display.fillTriangle(cx+1, cy+3, cx+6, cy+1, cx+6, cy+5, WHITE); // LEFT
            
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("ESCAPED!");
            display.setCursor(35, 35); display.print("Moves: "); display.print(moves);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(MazeGame)
