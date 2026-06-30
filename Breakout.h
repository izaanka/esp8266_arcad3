#pragma once

class BreakoutGame : public ArcadeGame {
private:
    float ballX, ballY, ballVX, ballVY;
    int paddleX, paddleW = 24;
    bool blocks[4][12]; // 4 rows, 12 columns
    int blocksRemaining = 0;
    int state = 20;

public:
    const char* getName() override { return "Breakout"; }

    void init() override {
        ballX = 64; ballY = 40;
        ballVX = 1.8; ballVY = -1.8;
        paddleX = 52;
        blocksRemaining = 0;
        
        for(int y = 0; y < 4; y++) {
          for(int x = 0; x < 12; x++) {
            if (random(100) > 30) { 
              blocks[y][x] = true;
              blocksRemaining++;
            } else {
              blocks[y][x] = false;
            }
          }
        }
        state = 20;
    }

    void update() override {
        if (state == 20) { 
            if (digitalRead(btnLeft) == LOW) paddleX -= 3;
            if (digitalRead(btnRight) == LOW) paddleX += 3;
            
            if (paddleX < 0) paddleX = 0;
            if (paddleX > 128 - paddleW) paddleX = 128 - paddleW;
        
            ballX += ballVX;
            ballY += ballVY;
            if (ballX <= 0 || ballX >= 127) ballVX = -ballVX;
            if (ballY <= 0) ballVY = -ballVY;
            
            if (ballY >= 56 && ballY <= 60 && ballX >= paddleX && ballX <= paddleX + paddleW) {
              ballVY = -ballVY;
              ballY = 55; 
            }
        
            if (ballY < 24) { 
              int bX_idx = ballX / 11;
              int bY_idx = ballY / 6;  
              
              if (bX_idx >= 0 && bX_idx < 12 && bY_idx >= 0 && bY_idx < 4) {
                if (blocks[bY_idx][bX_idx]) {
                  blocks[bY_idx][bX_idx] = false;
                  ballVY = -ballVY;               
                  blocksRemaining--;
                }
              }
            }
        
            if (ballY > 64 || blocksRemaining == 0) {
              state = 21;
              delay(500); 
            }
        
            display.clearDisplay();
            for(int y = 0; y < 4; y++) {
              for(int x = 0; x < 12; x++) {
                if (blocks[y][x]) { display.fillRect(x * 11, y * 6, 9, 4, WHITE); }
              }
            }
            
            display.fillRect(paddleX, 58, paddleW, 3, WHITE);
            display.fillRect(ballX, ballY, 2, 2, WHITE);
            display.display();
            delay(15); 
        }
        else if (state == 21) { 
            display.clearDisplay();
            display.setCursor(20, 20);
            if (blocksRemaining == 0) display.print("YOU WIN!");
            else display.print("GAME OVER");
            
            display.setCursor(20, 40); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(BreakoutGame)
