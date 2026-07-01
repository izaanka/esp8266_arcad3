#pragma once

class MinerGame : public ArcadeGame {
private:
    int grid[16][8]; // 16x8 grid, cells are 8x8 pixels. 0=empty, 1=dirt, 2=gem, 3=rock
    int px, py;
    int score;
    int state;
    bool lastL, lastS, lastR;

    void genRow(int y) {
        for(int x=0; x<16; x++) {
            if (random(100) < 10) grid[x][y] = 3; // rock
            else if (random(100) < 15) grid[x][y] = 2; // gem
            else grid[x][y] = 1; // dirt
        }
    }

public:
    const char* getCategory() override { return "Action"; }
    const char* getName() override { return "Miner"; }
    int getScore() override { return score; }

    void init() override {
        px = 8; py = 0; score = 0; state = 0;
        lastL = true; lastS = true; lastR = true;
        for(int y=0; y<8; y++) genRow(y);
        grid[px][py] = 0;
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);

            int nx = px; int ny = py;
            if (cL && !lastL) nx--;
            if (cR && !lastR) nx++;
            if (cS && !lastS) ny++;

            if (nx >= 0 && nx < 16 && ny < 8) {
                if (grid[nx][ny] != 3) { // not a rock
                    if (grid[nx][ny] == 2) score++;
                    grid[nx][ny] = 0;
                    px = nx; py = ny;
                }
            }
            
            // Scroll down logic if at bottom
            if (py >= 6) {
                for(int y=0; y<7; y++) {
                    for(int x=0; x<16; x++) grid[x][y] = grid[x][y+1];
                }
                genRow(7);
                py--;
            }

            // Rock falling logic
            if (random(100) < 20) {
                for(int y=6; y>=0; y--) {
                    for(int x=0; x<16; x++) {
                        if (grid[x][y] == 3 && grid[x][y+1] == 0) {
                            grid[x][y] = 0; grid[x][y+1] = 3;
                            if (px == x && py == y+1) { state = 1; delay(500); } // Crushed!
                        }
                    }
                }
            }

            lastL = cL; lastS = cS; lastR = cR;

            display.clearDisplay();
            for(int x=0; x<16; x++) {
                for(int y=0; y<8; y++) {
                    int dx = x*8; int dy = y*8;
                    if (grid[x][y] == 1) { // dirt
                        display.drawPixel(dx+2, dy+2, WHITE);
                        display.drawPixel(dx+6, dy+5, WHITE);
                    } else if (grid[x][y] == 2) { // gem
                        display.drawRect(dx+2, dy+2, 4, 4, WHITE);
                    } else if (grid[x][y] == 3) { // rock
                        display.fillCircle(dx+4, dy+4, 3, WHITE);
                    }
                }
            }
            
            // Player
            display.fillRect(px*8+1, py*8+1, 6, 6, WHITE);

            display.fillRect(0,0,30,10,BLACK);
            display.setCursor(0,0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("CRUSHED!");
            display.setCursor(35, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(MinerGame)
