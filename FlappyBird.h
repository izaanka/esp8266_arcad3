#pragma once

class FlappyBirdGame : public ArcadeGame {
private:
    float playerY = 32, playerVY = 0;
    const float gravity = 0.4;
    const float jumpForce = -3.5;
    
    int pipeX = 128;
    int pipeGapY = 32;
    int pipeWidth = 10;
    int pipeGap = 26;
    
    int score = 0;
    int state = 100; // 100 = playing, 101 = game over
    bool lastSelect = true;

public:
    const char* getCategory() override { return "Arcade"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Flappy Bird"; }

    void init() override {
        playerY = 32;
        playerVY = 0;
        pipeX = 128;
        pipeGapY = random(20, 44);
        score = 0;
        state = 100;
        lastSelect = true;
    }

    void update() override {
        if (state == 100) {
            bool currSelect = digitalRead(btnSelect);
            if (currSelect == LOW && lastSelect == HIGH) {
                playerVY = jumpForce;
            }
            lastSelect = currSelect;

            playerVY += gravity;
            playerY += playerVY;

            pipeX -= 2; // Pipe speed
            
            if (pipeX < -pipeWidth) {
                pipeX = 128;
                pipeGapY = random(20, 44);
                score++;
            }

            // Collision detection
            int pTop = playerY;
            int pBottom = playerY + 4;
            int pLeft = 20;
            int pRight = 24;

            bool hitFloorOrCeil = (playerY < 0 || playerY > 60);
            bool hitPipe = false;

            if (pRight > pipeX && pLeft < pipeX + pipeWidth) {
                if (pTop < pipeGapY - pipeGap/2 || pBottom > pipeGapY + pipeGap/2) {
                    hitPipe = true;
                }
            }

            if (hitFloorOrCeil || hitPipe) {
                state = 101;
                delay(500);
            }

            display.clearDisplay();
            
            // Draw player
            display.fillRect(pLeft, playerY, 4, 4, WHITE);
            
            // Draw pipes
            display.fillRect(pipeX, 0, pipeWidth, pipeGapY - pipeGap/2, WHITE);
            display.fillRect(pipeX, pipeGapY + pipeGap/2, pipeWidth, 64 - (pipeGapY + pipeGap/2), WHITE);

            display.setCursor(0, 0);
            display.print(score);
            
            display.display();
            delay(15);
        } else if (state == 101) {
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

REGISTER_GAME(FlappyBirdGame)
