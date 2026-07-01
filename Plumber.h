#pragma once

class PlumberGame : public ArcadeGame {
private:
    int grid[3][3]; // 0= |, 1= -, 2= L, 3= r, 4= ¬, 5= J (simple representation)
    // Actually simpler: just bool connected Left, Right, Up, Down for each tile.
    struct Tile { bool U, D, L, R; };
    Tile tiles[3][3];
    int cx, cy;
    int state;
    int score;
    bool lastS;
    float water;

    void rotateTile(int x, int y) {
        bool temp = tiles[x][y].U;
        tiles[x][y].U = tiles[x][y].L;
        tiles[x][y].L = tiles[x][y].D;
        tiles[x][y].D = tiles[x][y].R;
        tiles[x][y].R = temp;
    }

    void loadLevel() {
        for(int x=0; x<3; x++) {
            for(int y=0; y<3; y++) {
                tiles[x][y] = {random(2), random(2), random(2), random(2)};
                // Ensure at least one connection
                if (!tiles[x][y].U && !tiles[x][y].D && !tiles[x][y].L && !tiles[x][y].R) {
                    tiles[x][y].L = true; tiles[x][y].R = true;
                }
            }
        }
        water = 0; state = 0;
    }

    bool checkWin() {
        // Simple pathfinding from left (0,1) to right (2,1)
        // This is a simplified check for a specific path, but for a 3x3 we can do basic floodfill
        bool visited[3][3] = {false};
        int qx[9], qy[9];
        int head=0, tail=0;
        
        if (!tiles[0][1].L) return false; // must enter middle left
        
        qx[tail]=0; qy[tail]=1; tail++; visited[0][1] = true;
        
        while(head < tail) {
            int x = qx[head]; int y = qy[head]; head++;
            if (x == 2 && y == 1 && tiles[x][y].R) return true; // Reached exit
            
            if (tiles[x][y].U && y>0 && !visited[x][y-1] && tiles[x][y-1].D) {
                visited[x][y-1] = true; qx[tail]=x; qy[tail]=y-1; tail++;
            }
            if (tiles[x][y].D && y<2 && !visited[x][y+1] && tiles[x][y+1].U) {
                visited[x][y+1] = true; qx[tail]=x; qy[tail]=y+1; tail++;
            }
            if (tiles[x][y].L && x>0 && !visited[x-1][y] && tiles[x-1][y].R) {
                visited[x-1][y] = true; qx[tail]=x-1; qy[tail]=y; tail++;
            }
            if (tiles[x][y].R && x<2 && !visited[x+1][y] && tiles[x+1][y].L) {
                visited[x+1][y] = true; qx[tail]=x+1; qy[tail]=y; tail++;
            }
        }
        return false;
    }

public:
    const char* getCategory() override { return "Puzzle"; }
    const char* getName() override { return "Plumber"; }
    int getScore() override { return score; }

    void init() override {
        cx = 0; cy = 0; score = 0; lastS = true;
        loadLevel();
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) { cx--; if(cx<0){cx=2; cy--; if(cy<0)cy=2;} delay(150); }
            if (digitalRead(btnRight) == LOW) { cx++; if(cx>2){cx=0; cy++; if(cy>2)cy=0;} delay(150); }
            
            bool cS = (digitalRead(btnSelect) == LOW);
            if (cS && !lastS) {
                rotateTile(cx, cy);
                if (checkWin()) {
                    score++;
                    loadLevel();
                }
            }
            lastS = cS;

            water += 0.2;
            if (water > 100) { state = 1; delay(500); }

            display.clearDisplay();
            
            // Draw grid
            for(int x=0; x<3; x++) {
                for(int y=0; y<3; y++) {
                    int px = 40 + x*16; int py = 10 + y*16;
                    display.drawRect(px, py, 14, 14, WHITE);
                    
                    int cx = px+7; int cy = py+7;
                    if (tiles[x][y].U) display.drawLine(cx, cy, cx, py, WHITE);
                    if (tiles[x][y].D) display.drawLine(cx, cy, cx, py+14, WHITE);
                    if (tiles[x][y].L) display.drawLine(cx, cy, px, cy, WHITE);
                    if (tiles[x][y].R) display.drawLine(cx, cy, px+14, cy, WHITE);
                }
            }
            
            // Cursor
            display.drawRect(39 + cx*16, 9 + cy*16, 16, 16, WHITE);
            
            // In/Out pipes
            display.drawLine(20, 33, 40, 33, WHITE);
            display.drawLine(88, 33, 108, 33, WHITE);
            
            // Water bar
            display.drawLine(10, 60, 118, 60, WHITE);
            display.fillRect(10, 58, (int)water, 4, WHITE);
            
            display.setCursor(0,0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(40, 20); display.print("LEAK!");
            display.setCursor(40, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(PlumberGame)
