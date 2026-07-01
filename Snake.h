#pragma once

class SnakeGame : public ArcadeGame {
private:
    int snakeX[100];
    int snakeY[100];
    int snakeLength;
    int dir; // 0: Up, 1: Right, 2: Down, 3: Left
    int nextDir;
    int appleX, appleY;
    int score;
    int state; // 0: playing, 1: game over
    bool lastLeft = true;
    bool lastRight = true;

    void spawnApple() {
        bool valid = false;
        while (!valid) {
            appleX = random(0, 32);
            appleY = random(0, 14); // 0 to 13, leaves top 8 pixels for score (y*4 + 8)
            valid = true;
            for (int i = 0; i < snakeLength; i++) {
                if (snakeX[i] == appleX && snakeY[i] == appleY) {
                    valid = false;
                    break;
                }
            }
        }
    }

public:
    const char* getCategory() override { return "Arcade"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Snake"; }

    void init() override {
        snakeLength = 4;
        dir = 1; // Start moving right
        nextDir = 1;
        for (int i = 0; i < snakeLength; i++) {
            snakeX[i] = 16 - i;
            snakeY[i] = 7;
        }
        score = 0;
        state = 0;
        spawnApple();
        lastLeft = true;
        lastRight = true;
    }

    void update() override {
        if (state == 0) {
            int gameSpeed = 120 - (score * 3);
            if (gameSpeed < 30) gameSpeed = 30;
            
            // Poll for inputs during the delay to ensure responsiveness
            unsigned long startWait = millis();
            while(millis() - startWait < (unsigned long)gameSpeed) {
                bool currentLeft = digitalRead(btnLeft);
                bool currentRight = digitalRead(btnRight);
                
                if (currentLeft == LOW && lastLeft == HIGH) {
                    nextDir = (dir + 3) % 4; // Turn Left (counter-clockwise)
                }
                if (currentRight == LOW && lastRight == HIGH) {
                    nextDir = (dir + 1) % 4; // Turn Right (clockwise)
                }
                lastLeft = currentLeft;
                lastRight = currentRight;
                delay(5);
            }
            
            dir = nextDir;
            
            // Move body
            for (int i = snakeLength - 1; i > 0; i--) {
                snakeX[i] = snakeX[i - 1];
                snakeY[i] = snakeY[i - 1];
            }
            
            // Move head
            if (dir == 0) snakeY[0]--;
            else if (dir == 1) snakeX[0]++;
            else if (dir == 2) snakeY[0]++;
            else if (dir == 3) snakeX[0]--;
            
            // Check collision with walls
            if (snakeX[0] < 0 || snakeX[0] >= 32 || snakeY[0] < 0 || snakeY[0] >= 14) {
                state = 1;
                delay(500);
            }
            
            // Check collision with self
            for (int i = 1; i < snakeLength; i++) {
                if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
                    state = 1;
                    delay(500);
                }
            }
            
            // Check apple
            if (snakeX[0] == appleX && snakeY[0] == appleY) {
                score++;
                if (snakeLength < 100) {
                    snakeX[snakeLength] = snakeX[snakeLength - 1];
                    snakeY[snakeLength] = snakeY[snakeLength - 1];
                    snakeLength++;
                }
                spawnApple();
            }
            
            // Rendering
            display.clearDisplay();
            
            // Score
            display.setCursor(0, 0);
            display.print("Score: ");
            display.print(score);
            
            // Draw apple
            display.fillRect(appleX * 4, appleY * 4 + 8, 4, 4, WHITE);
            
            // Draw snake
            for (int i = 0; i < snakeLength; i++) {
                if (i == 0) {
                    // Head: solid block
                    display.fillRect(snakeX[i] * 4, snakeY[i] * 4 + 8, 4, 4, WHITE);
                } else {
                    // Body: hollow block for contrast, or just slightly smaller
                    display.fillRect(snakeX[i] * 4 + 1, snakeY[i] * 4 + 9, 2, 2, WHITE);
                }
            }
            
            // Draw border line separating score from game area
            display.drawLine(0, 7, 128, 7, WHITE);
            
            display.display();
            
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("GAME OVER");
            display.setCursor(35, 35); display.print("Score: "); display.print(score);
            display.setCursor(20, 50); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(SnakeGame)
