#pragma once

class BombermanGame : public ArcadeGame {
private:
    int grid[11][7]; // 0=empty, 1=solid, 2=destructible, 3=bomb, 4=explosion
    int px, py;
    int bombTimer;
    int score;
    int state;
    bool lastL, lastR, lastS;

    void generateGrid() {
        for(int x=0; x<11; x++) {
            for(int y=0; y<7; y++) {
                if (x%2==1 && y%2==1) grid[x][y] = 1; // solid blocks
                else if (random(100) < 30) grid[x][y] = 2; // breakable
                else grid[x][y] = 0;
            }
        }
        grid[0][0] = 0; grid[1][0] = 0; grid[0][1] = 0; // safe zone
    }

public:
    const char* getCategory() override { return "Puzzle"; }
    const char* getName() override { return "Bomberman"; }
    int getScore() override { return score; }

    void init() override {
        px = 0; py = 0; bombTimer = 0; score = 0; state = 0;
        lastL = true; lastR = true; lastS = true;
        generateGrid();
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);

            // Move (L/R moves horizontally, but we need 4-way. Let's make L/R cycle direction and S move forward)
            // Or simpler for 3 buttons: L=Left, R=Right, S=Drop bomb if none, else S=Move Down, Long S=Move Up? 
            // Better: Auto-move forward. L/R steers. S drops bomb.
            // Actually, a simpler Bomberman mapping:
            static int face = 0; // 0=R, 1=D, 2=L, 3=U
            if (cL && !lastL) { face--; if (face<0) face=3; }
            if (cR && !lastR) { face++; if (face>3) face=0; }
            
            static int moveDelay = 0;
            if (moveDelay > 0) moveDelay--;
            else if (cS) { // Hold S to move forward. Tap to drop bomb?
                // Too clunky. Let's do: L/R moves X, S drops bomb. How to move Y?
                // Auto ping-pong Y! 
            }
            
            // Let's do: L/R moves horizontally. S moves vertically. Auto flip direction on collision.
            static int dirY = 1;
            if (cL && !lastL) { int nx = px - 1; if (nx>=0 && grid[nx][py] <= 0) px = nx; }
            if (cR && !lastR) { int nx = px + 1; if (nx<11 && grid[nx][py] <= 0) px = nx; }
            if (cS && !lastS) { 
                int ny = py + dirY;
                if (ny<0 || ny>=7 || grid[px][ny] > 0) dirY = -dirY; // flip and try again
                ny = py + dirY;
                if (ny>=0 && ny<7 && grid[px][ny] <= 0) py = ny; 
                else {
                    // Try drop bomb if stuck?
                    if (bombTimer == 0) { grid[px][py] = 3; bombTimer = 30; }
                }
            }
            // Double press S to drop bomb? Just hold S to drop bomb.
            static int sHold = 0;
            if (cS) sHold++; else sHold = 0;
            if (sHold > 15 && bombTimer == 0) { grid[px][py] = 3; bombTimer = 30; sHold=0; }

            // Explosion logic
            for(int x=0; x<11; x++) for(int y=0; y<7; y++) if (grid[x][y] == 4) grid[x][y] = 0; // clear flames

            if (bombTimer > 0) {
                bombTimer--;
                if (bombTimer == 0) {
                    // Explode
                    for(int x=0; x<11; x++) {
                        for(int y=0; y<7; y++) {
                            if (grid[x][y] == 3) {
                                grid[x][y] = 4;
                                // Expand + shape
                                if (x>0 && grid[x-1][y] != 1) { if(grid[x-1][y]==2) score++; grid[x-1][y]=4; }
                                if (x<10 && grid[x+1][y] != 1) { if(grid[x+1][y]==2) score++; grid[x+1][y]=4; }
                                if (y>0 && grid[x][y-1] != 1) { if(grid[x][y-1]==2) score++; grid[x][y-1]=4; }
                                if (y<6 && grid[x][y+1] != 1) { if(grid[x][y+1]==2) score++; grid[x][y+1]=4; }
                            }
                        }
                    }
                }
            }

            if (grid[px][py] == 4) { state = 1; delay(500); } // blew yourself up

            lastL = cL; lastR = cR; lastS = cS;

            display.clearDisplay();
            for(int x=0; x<11; x++) {
                for(int y=0; y<7; y++) {
                    int dx = 20 + x*8; int dy = 4 + y*8;
                    if (grid[x][y] == 1) display.fillRect(dx, dy, 7, 7, WHITE); // solid
                    if (grid[x][y] == 2) display.drawRect(dx, dy, 7, 7, WHITE); // soft
                    if (grid[x][y] == 3) display.fillCircle(dx+3, dy+3, 3, WHITE); // bomb
                    if (grid[x][y] == 4) { // flame
                        display.drawLine(dx, dy, dx+6, dy+6, WHITE);
                        display.drawLine(dx+6, dy, dx, dy+6, WHITE);
                    }
                }
            }
            
            // Player
            display.fillRect(20+px*8+1, 4+py*8+1, 5, 5, WHITE);

            display.setCursor(0,0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("BOOM!");
            display.setCursor(30, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(BombermanGame)
