#pragma once

class LaserReflectGame : public ArcadeGame {
private:
    int grid[6][6]; // 0=empty, 1= / mirror, 2= \ mirror, 3=block, 4=target
    int cx, cy;
    int state; // 0=edit, 1=fire, 2=win
    int score;

    void loadLevel() {
        for(int x=0; x<6; x++) for(int y=0; y<6; y++) grid[x][y] = 0;
        cx = 3; cy = 3;
        grid[5][random(0,6)] = 4; // Target on right
        for(int i=0; i<3; i++) grid[random(1,5)][random(0,6)] = 3; // Blocks
    }

public:
    const char* getCategory() override { return "Action"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Laser Reflect"; }

    void init() override { score = 0; state = 0; loadLevel(); }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) { cx--; if (cx<1) cx=5; delay(150); }
            if (digitalRead(btnRight) == LOW) { cy--; if (cy<0) cy=5; delay(150); }
            if (digitalRead(btnSelect) == LOW) {
                if (grid[cx][cy] == 0) grid[cx][cy] = 1;
                else if (grid[cx][cy] == 1) grid[cx][cy] = 2;
                else if (grid[cx][cy] == 2) grid[cx][cy] = 0;
                delay(150);
            }
            // Hold Select to fire (cheap check: count frames held)
            static int hold = 0;
            if (digitalRead(btnSelect) == LOW) hold++; else hold=0;
            if (hold > 20) { state = 1; hold = 0; delay(200); }
        }

        display.clearDisplay();
        
        // Draw grid
        for(int x=0; x<6; x++) {
            for(int y=0; y<6; y++) {
                int px = 20 + x*12; int py = 2 + y*10;
                if (grid[x][y] == 3) display.fillRect(px, py, 10, 8, WHITE);
                if (grid[x][y] == 4) display.drawCircle(px+5, py+4, 4, WHITE);
                if (grid[x][y] == 1) display.drawLine(px, py+8, px+10, py, WHITE); // /
                if (grid[x][y] == 2) display.drawLine(px, py, px+10, py+8, WHITE); // Backslash
            }
        }

        if (state == 0) display.drawRect(20+cx*12, 2+cy*10, 10, 8, WHITE);

        // Raycast
        if (state == 1 || state == 2) {
            int rx = 0; int ry = 0; int dx = 1; int dy = 0;
            bool hitTarget = false; bool hitWall = false;
            while(!hitTarget && !hitWall) {
                int px1 = 20 + rx*12 + 5; int py1 = 2 + ry*10 + 4;
                if (grid[rx][ry] == 1) { int t = dx; dx = -dy; dy = -t; }
                if (grid[rx][ry] == 2) { int t = dx; dx = dy; dy = t; }
                
                rx += dx; ry += dy;
                if (rx < 0 || rx > 5 || ry < 0 || ry > 5 || grid[rx][ry] == 3) { hitWall = true; break; }
                if (grid[rx][ry] == 4) { hitTarget = true; break; }
                
                int px2 = 20 + rx*12 + 5; int py2 = 2 + ry*10 + 4;
                display.drawLine(px1, py1, px2, py2, WHITE);
            }
            
            if (hitTarget) {
                display.setCursor(0,0); display.print("WIN!");
                if (state == 1) { state = 2; score++; delay(1000); loadLevel(); state=0; }
            } else {
                display.setCursor(0,0); display.print("MISS!");
                if (state == 1) { state = 2; delay(1000); state=0; }
            }
        }
        
        display.setCursor(0, 50); display.print("S:"); display.print(score);
        display.display();
        delay(15);
    }
};

REGISTER_GAME(LaserReflectGame)
