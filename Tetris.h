#pragma once

class TetrisGame : public ArcadeGame {
private:
    const uint16_t tetrominos[7][4] = {
      { 0x0F00, 0x2222, 0x00F0, 0x4444 }, // I
      { 0x8E00, 0x6440, 0x0E20, 0x44C0 }, // J
      { 0x2E00, 0x4460, 0x0E80, 0xC440 }, // L
      { 0xCC00, 0xCC00, 0xCC00, 0xCC00 }, // O
      { 0x6C00, 0x4620, 0x06C0, 0x8C40 }, // S
      { 0x4E00, 0x4640, 0x0E40, 0x4C40 }, // T
      { 0xC600, 0x2640, 0x0C60, 0x4C80 }  // Z
    };

    uint16_t board[20];
    int cx, cy, type, rot;
    int score, linesCleared, level;
    int state = 0; // 0 = Playing, 1 = Game Over
    
    unsigned long lastDropTime = 0;
    unsigned long rightPressTime = 0;
    int dropDelay = 500;

    bool lastLeft = true, lastRight = true, lastSelect = true;

    bool checkCollision(int tx, int ty, int tType, int tRot) {
        uint16_t shape = tetrominos[tType][tRot];
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                if (shape & (0x8000 >> (r * 4 + c))) {
                    int nx = tx + c;
                    int ny = ty + r;
                    if (nx < 0 || nx >= 10 || ny >= 20) return true;
                    if (ny >= 0 && (board[ny] & (1 << nx))) return true;
                }
            }
        }
        return false;
    }

    void spawnPiece() {
        type = random(7);
        rot = 0;
        cx = 3;
        cy = -2;
        if (checkCollision(cx, cy, type, rot)) {
            state = 1; // Game Over
        }
    }

    void lockPiece() {
        uint16_t shape = tetrominos[type][rot];
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                if (shape & (0x8000 >> (r * 4 + c))) {
                    int ny = cy + r;
                    if (ny >= 0 && ny < 20) {
                        board[ny] |= (1 << (cx + c));
                    }
                }
            }
        }

        // Clear lines
        int lines = 0;
        for (int r = 19; r >= 0; r--) {
            if ((board[r] & 0x3FF) == 0x3FF) {
                for (int i = r; i > 0; i--) {
                    board[i] = board[i-1];
                }
                board[0] = 0;
                lines++;
                r++; // Check same row again
            }
        }
        
        if (lines > 0) {
            linesCleared += lines;
            score += (lines * lines) * 100;
            level = linesCleared / 5 + 1;
        }
        
        spawnPiece();
    }

public:
    const char* getName() override { return "Tetris"; }

    void init() override {
        for(int i=0; i<20; i++) board[i] = 0;
        score = 0;
        linesCleared = 0;
        level = 1;
        dropDelay = 500;
        state = 0;
        lastLeft = lastRight = lastSelect = true;
        spawnPiece();
        lastDropTime = millis();
    }

    void update() override {
        if (state == 0) {
            bool currLeft = digitalRead(btnLeft);
            bool currRight = digitalRead(btnRight);
            bool currSelect = digitalRead(btnSelect);

            if (currLeft == LOW && lastLeft == HIGH) {
                if (!checkCollision(cx - 1, cy, type, rot)) cx--;
            }
            
            if (currRight == LOW) {
                if (lastRight == HIGH) {
                    if (!checkCollision(cx + 1, cy, type, rot)) cx++;
                    rightPressTime = millis();
                } else {
                    if (millis() - rightPressTime > 400) { // Hard drop threshold
                        while (!checkCollision(cx, cy + 1, type, rot)) cy++;
                        lockPiece();
                        rightPressTime = millis() + 5000; // Prevent repeating until button released
                    }
                }
            }
            
            if (currSelect == LOW && lastSelect == HIGH) {
                int nextRot = (rot + 1) % 4;
                if (!checkCollision(cx, cy, type, nextRot)) rot = nextRot;
            }

            lastLeft = currLeft;
            lastRight = currRight;
            lastSelect = currSelect;

            if (millis() - lastDropTime > dropDelay) {
                if (!checkCollision(cx, cy + 1, type, rot)) {
                    cy++;
                } else {
                    lockPiece();
                }
                lastDropTime = millis();
            }

            // Draw
            display.clearDisplay();
            
            // Draw Playfield Boundary
            int offsetX = 49;
            int offsetY = 2;
            display.drawRect(offsetX - 2, offsetY - 2, 34, 64, WHITE);
            
            // Draw Board
            for (int r = 0; r < 20; r++) {
                for (int c = 0; c < 10; c++) {
                    if (board[r] & (1 << c)) {
                        display.fillRect(offsetX + c*3, offsetY + r*3, 2, 2, WHITE);
                    }
                }
            }

            // Draw Current Piece
            uint16_t shape = tetrominos[type][rot];
            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {
                    if (shape & (0x8000 >> (r * 4 + c))) {
                        int py = cy + r;
                        int px = cx + c;
                        if (py >= 0) {
                            display.fillRect(offsetX + px*3, offsetY + py*3, 2, 2, WHITE);
                        }
                    }
                }
            }
            
            // Draw UI
            display.setTextSize(1);
            display.setTextColor(WHITE);
            display.setCursor(0, 10);
            display.print("Score:");
            display.setCursor(0, 20);
            display.print(score);
            
            display.setCursor(0, 40);
            display.print("Lvl:");
            display.setCursor(0, 50);
            display.print(level);

            display.display();
            delay(15);
        }
        else if (state == 1) {
            display.clearDisplay();
            display.setCursor(20, 20); display.print("GAME OVER");
            display.setCursor(20, 35); display.print("Score: ");
            display.print(score);
            display.setCursor(20, 50); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(TetrisGame)
