#pragma once

class PacmanGame : public ArcadeGame {
private:
    const byte initialMaze[8][16] = {
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,2,2,2,2,2,2,1,1,2,2,2,2,2,2,1},
      {1,2,1,1,1,1,2,1,1,2,1,1,1,1,2,1},
      {1,2,2,2,1,2,2,2,2,2,2,1,2,2,2,1},
      {1,2,1,2,1,2,1,1,1,1,2,1,2,1,2,1},
      {1,2,1,2,2,2,2,2,2,2,2,2,2,1,2,1},
      {1,2,2,2,1,1,1,1,1,1,1,1,2,2,2,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };
    
    byte maze[8][16];
    
    int px, py;
    int pDir; // 0=Right, 1=Down, 2=Left, 3=Up
    int dx[4] = {1, 0, -1, 0};
    int dy[4] = {0, 1, 0, -1};
    
    int gx, gy;
    
    int score;
    int pillsLeft;
    int state = 200; // 200=Play, 201=Over, 202=Win
    
    unsigned long lastMoveTime = 0;
    unsigned long lastGhostTime = 0;
    int moveDelay = 200;
    int ghostDelay = 350;
    
    bool lastLeft = true, lastRight = true;

public:
    const char* getCategory() override { return "Arcade"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Pacman"; }

    void init() override {
        pillsLeft = 0;
        score = 0;
        for(int y=0; y<8; y++) {
            for(int x=0; x<16; x++) {
                maze[y][x] = initialMaze[y][x];
                if (maze[y][x] == 2) pillsLeft++;
            }
        }
        px = 1; py = 1;
        pDir = 0;
        gx = 14; gy = 6;
        state = 200;
        lastLeft = lastRight = true;
        lastMoveTime = millis();
        lastGhostTime = millis();
    }

    void update() override {
        if (state == 200) {
            bool currLeft = digitalRead(btnLeft);
            bool currRight = digitalRead(btnRight);
            
            // Controls: Left turns CCW, Right turns CW
            if (currLeft == LOW && lastLeft == HIGH) {
                pDir = (pDir + 3) % 4; 
            }
            if (currRight == LOW && lastRight == HIGH) {
                pDir = (pDir + 1) % 4; 
            }
            lastLeft = currLeft;
            lastRight = currRight;

            // Move Pacman
            if (millis() - lastMoveTime > moveDelay) {
                int nx = px + dx[pDir];
                int ny = py + dy[pDir];
                if (maze[ny][nx] != 1) { // Not a wall
                    px = nx;
                    py = ny;
                    if (maze[py][px] == 2) { // Ate a pill
                        maze[py][px] = 0;
                        score++;
                        pillsLeft--;
                    }
                }
                lastMoveTime = millis();
            }

            // Move Ghost
            if (millis() - lastGhostTime > ghostDelay) {
                int bestDist = 999;
                int bestDir = -1;
                for(int i=0; i<4; i++) {
                    int nx = gx + dx[i];
                    int ny = gy + dy[i];
                    if (maze[ny][nx] != 1) { // Not a wall
                        int dist = abs(nx - px) + abs(ny - py);
                        if (dist < bestDist) {
                            bestDist = dist;
                            bestDir = i;
                        }
                    }
                }
                if (bestDir != -1) {
                    gx += dx[bestDir];
                    gy += dy[bestDir];
                }
                lastGhostTime = millis();
            }

            // Check collisions and win condition
            if (px == gx && py == gy) {
                state = 201; // Lose
                delay(500);
            } else if (pillsLeft == 0) {
                state = 202; // Win
                delay(500);
            }

            // Draw
            display.clearDisplay();
            
            // Draw Maze
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 16; x++) {
                    if (maze[y][x] == 1) {
                        display.drawRect(x*8, y*8, 8, 8, WHITE);
                    } else if (maze[y][x] == 2) {
                        display.fillRect(x*8 + 3, y*8 + 3, 2, 2, WHITE);
                    }
                }
            }

            // Draw Pacman
            display.fillCircle(px*8 + 4, py*8 + 4, 3, WHITE);

            // Draw Ghost
            display.fillRect(gx*8 + 1, gy*8 + 1, 6, 6, WHITE);

            display.display();
            delay(15);
            
        } else if (state == 201 || state == 202) {
            display.clearDisplay();
            display.setCursor(20, 15);
            if (state == 201) display.print("GAME OVER");
            else display.print("YOU WIN!");
            
            display.setCursor(20, 30); display.print("Score: ");
            display.print(score);
            display.setCursor(20, 45); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(PacmanGame)
