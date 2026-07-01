#pragma once

class MinesweeperGame : public ArcadeGame {
private:
    byte msGrid[4][8]; // Bit 0: Mine, Bit 1: Revealed, Bit 2: Flagged, Bits 4-7: NeighborMineCount
    int msCursor = 0;
    bool msGameOver = false;
    int msRevealedCount = 0;
    int state = 49;
    int difficulty = 1; // 0=Easy, 1=Medium, 2=Hard

    void revealTile(int x, int y) {
      if (x < 0 || x >= 8 || y < 0 || y >= 4) return;
      if (msGrid[y][x] & 0x02) return; // Already revealed
      if (msGrid[y][x] & 0x04) return; // Flagged
      
      msGrid[y][x] |= 0x02; // Set Revealed Bit
      msRevealedCount++;
      if (msGrid[y][x] & 0x01) {
        msGameOver = true;
        state = 51; 
        return;
      }
      
      int count = msGrid[y][x] >> 4;
      if (count == 0) {
        for (int dy = -1; dy <= 1; dy++) {
          for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            revealTile(x + dx, y + dy);
          }
        }
      }
    }

public:
    const char* getCategory() override { return "Puzzle"; }
    const char* getName() override { return "Minesweeper"; }

    void init() override {
        msGameOver = false;
        msRevealedCount = 0;
        msCursor = 0;
        state = 49;
        difficulty = 1;
    }

    void generateBoard() {
        for (int y = 0; y < 4; y++) {
          for (int x = 0; x < 8; x++) {
            msGrid[y][x] = 0;
          }
        }
        
        int mineCount = (difficulty == 0) ? 4 : (difficulty == 1) ? 6 : 9;
        int placed = 0;
        while (placed < mineCount) {
          int rx = random(8);
          int ry = random(4);
          if (!(msGrid[ry][rx] & 0x01)) {
            msGrid[ry][rx] |= 0x01; 
            placed++;
          }
        }
        
        for (int y = 0; y < 4; y++) {
          for (int x = 0; x < 8; x++) {
            if (msGrid[y][x] & 0x01) continue;
            int count = 0;
            for (int dy = -1; dy <= 1; dy++) {
              for (int dx = -1; dx <= 1; dx++) {
                int ny = y + dy;
                int nx = x + dx;
                if (nx >= 0 && nx < 8 && ny >= 0 && ny < 4) {
                  if (msGrid[ny][nx] & 0x01) count++;
                }
              }
            }
            msGrid[y][x] |= (count << 4);
          }
        }
    }

    void update() override {
        if (state == 49) {
            display.clearDisplay();
            display.setCursor(25, 10); display.print("SELECT LEVEL");
            
            if (digitalRead(btnLeft) == LOW) { difficulty = (difficulty > 0) ? difficulty - 1 : 2; delay(150); }
            if (digitalRead(btnRight) == LOW) { difficulty = (difficulty < 2) ? difficulty + 1 : 0; delay(150); }
            
            display.setCursor(45, 30);
            if (difficulty == 0) display.print("EASY");
            else if (difficulty == 1) display.print("MEDIUM");
            else display.print("HARD");
            
            display.setCursor(20, 50); display.print("D6: Start");
            
            if (digitalRead(btnSelect) == LOW) {
                generateBoard();
                state = 50;
                delay(200);
            }
            display.display();
            delay(15);
        }
        else if (state == 50) {
            if (digitalRead(btnLeft) == LOW) { msCursor = (msCursor > 0) ? msCursor - 1 : 31; delay(120); }
            if (digitalRead(btnRight) == LOW) { msCursor = (msCursor < 31) ? msCursor + 1 : 0; delay(120); }
            
            if (digitalRead(btnSelect) == LOW) {
              int holdTime = 0;
              while (digitalRead(btnSelect) == LOW && holdTime < 10) { delay(50); holdTime++; }
              
              int cX = msCursor % 8;
              int cY = msCursor / 8;
              
              if (holdTime >= 10) {
                if (!(msGrid[cY][cX] & 0x02)) { msGrid[cY][cX] ^= 0x04; } // Toggle Flag
              } else {
                if (!(msGrid[cY][cX] & 0x04)) { 
                  revealTile(cX, cY);
                  if (!msGameOver) {
                    bool won = true;
                    for (int y = 0; y < 4; y++) {
                      for (int x = 0; x < 8; x++) {
                        if (!(msGrid[y][x] & 0x01) && !(msGrid[y][x] & 0x02)) { won = false; }
                      }
                    }
                    if (won) { state = 51; }
                  }
                }
              }
              while (digitalRead(btnSelect) == LOW) { delay(10); }
              delay(50);
            }
            
            display.clearDisplay();
            int offsetX = 16;
            int offsetY = 8;
            
            for (int y = 0; y < 4; y++) {
              for (int x = 0; x < 8; x++) {
                int drawX = offsetX + x * 12;
                int drawY = offsetY + y * 12;
                
                display.drawRect(drawX, drawY, 10, 10, WHITE);
                
                if (msGrid[y][x] & 0x02) {
                  if (msGrid[y][x] & 0x01) {
                    display.fillCircle(drawX + 5, drawY + 5, 3, WHITE); // Mine
                  } else {
                    int count = msGrid[y][x] >> 4;
                    if (count > 0) {
                      display.setCursor(drawX + 3, drawY + 1);
                      display.print(count);
                    } else {
                      display.drawPixel(drawX + 5, drawY + 5, WHITE);
                    }
                  }
                } else if (msGrid[y][x] & 0x04) {
                  display.drawLine(drawX + 2, drawY + 2, drawX + 8, drawY + 8, WHITE);
                  display.drawLine(drawX + 8, drawY + 2, drawX + 2, drawY + 8, WHITE); // Flag
                }
                
                if (msCursor == y * 8 + x) {
                  display.drawRect(drawX - 2, drawY - 2, 14, 14, WHITE);
                }
              }
            }
            display.display();
        }
        else if (state == 51) {
            display.clearDisplay();
            display.setCursor(20, 15);
            if (msGameOver) {
              display.print("BOMB DETONATED!");
              display.setCursor(20, 30);
              display.print("GAME OVER");
            } else {
              display.print("MINEFIELD CLEAR!");
              display.setCursor(20, 30);
              display.print("YOU WIN!");
            }
            display.setCursor(20, 50);
            display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(MinesweeperGame)
