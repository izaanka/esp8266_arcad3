#pragma once

class SokobanGame : public ArcadeGame {
private:
    // 0=floor, 1=wall, 2=box, 3=target, 4=player, 5=box on target
    int map[8][8];
    int initMap[8][8] = {
        {0,0,1,1,1,1,0,0},
        {0,0,1,3,0,1,0,0},
        {0,0,1,0,2,1,1,0},
        {1,1,1,2,0,0,1,0},
        {1,3,0,0,4,0,1,0},
        {1,1,1,1,0,0,1,0},
        {0,0,0,1,1,1,1,0},
        {0,0,0,0,0,0,0,0}
    };
    
    int pX, pY;
    int dir = 0; // 0=up, 1=right, 2=down, 3=left
    int state = 0;
    bool lastSelect = true;

    void loadMap() {
        for(int y=0; y<8; y++) {
            for(int x=0; x<8; x++) {
                map[y][x] = initMap[y][x];
                if (map[y][x] == 4) { pX = x; pY = y; map[y][x] = 0; }
            }
        }
        dir = 0; state = 0;
    }

public:
    const char* getCategory() override { return "Puzzle"; }
    const char* getName() override { return "Sokoban"; }

    void init() override {
        loadMap();
        lastSelect = true;
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) { dir = (dir + 3) % 4; delay(150); }
            if (digitalRead(btnRight) == LOW) { dir = (dir + 1) % 4; delay(150); }
            
            bool currSelect = (digitalRead(btnSelect) == LOW);
            if (currSelect && !lastSelect) {
                int dx = 0, dy = 0;
                if (dir == 0) dy = -1; else if (dir == 1) dx = 1;
                else if (dir == 2) dy = 1; else dx = -1;
                
                int nx = pX + dx, ny = pY + dy;
                if (map[ny][nx] == 0 || map[ny][nx] == 3) {
                    pX = nx; pY = ny; // move freely
                } else if (map[ny][nx] == 2 || map[ny][nx] == 5) {
                    // pushing a box
                    int nnx = nx + dx, nny = ny + dy;
                    if (map[nny][nnx] == 0 || map[nny][nnx] == 3) {
                        // box can move
                        if (map[ny][nx] == 5) map[ny][nx] = 3; else map[ny][nx] = 0;
                        if (map[nny][nnx] == 3) map[nny][nnx] = 5; else map[nny][nnx] = 2;
                        pX = nx; pY = ny;
                    }
                }
                
                // check win
                bool win = true;
                for(int y=0; y<8; y++) for(int x=0; x<8; x++) {
                    if (map[y][x] == 3) win = false; // unfilled target
                }
                if (win) state = 1;
            }
            lastSelect = currSelect;
            
            display.clearDisplay();
            
            int ox = 32, oy = 0;
            for(int y=0; y<8; y++) {
                for(int x=0; x<8; x++) {
                    int cx = ox + x*8; int cy = oy + y*8;
                    if (map[y][x] == 1) display.fillRect(cx, cy, 8, 8, WHITE);
                    if (map[y][x] == 2) { display.drawRect(cx, cy, 8, 8, WHITE); display.drawLine(cx,cy,cx+7,cy+7,WHITE); }
                    if (map[y][x] == 3) display.drawCircle(cx+4, cy+4, 2, WHITE);
                    if (map[y][x] == 5) display.fillRect(cx+1, cy+1, 6, 6, WHITE); // box on target
                }
            }
            
            // Draw Player
            int cx = ox + pX*8; int cy = oy + pY*8;
            if (dir == 0) display.fillTriangle(cx+4, cy, cx, cy+7, cx+7, cy+7, WHITE);
            if (dir == 1) display.fillTriangle(cx+7, cy+4, cx, cy, cx, cy+7, WHITE);
            if (dir == 2) display.fillTriangle(cx+4, cy+7, cx, cy, cx+7, cy, WHITE);
            if (dir == 3) display.fillTriangle(cx, cy+4, cx+7, cy, cx+7, cy+7, WHITE);
            
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("LEVEL CLEARED!");
            display.setCursor(20, 50); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(SokobanGame)
