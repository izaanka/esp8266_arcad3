#pragma once

class DoodleJumpGame : public ArcadeGame {
private:
    float pX = 60, pY = 40, pVY = 0;
    const float gravity = 0.2;
    const float jumpForce = -4.8; // increased jump force slightly for smoother feel
    
    float platX[6];
    float platY[6];
    
    float score = 0;
    int state = 0;

public:
    const char* getCategory() override { return "Arcade"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Sky Jumper"; }

    void init() override {
        pX = 60;
        pY = 32;
        pVY = jumpForce;
        score = 0;
        state = 0;
        
        platX[0] = 60; platY[0] = 60;
        platX[1] = random(0, 100); platY[1] = 45;
        platX[2] = random(0, 100); platY[2] = 30;
        platX[3] = random(0, 100); platY[3] = 15;
        platX[4] = random(0, 100); platY[4] = 0;
        platX[5] = random(0, 100); platY[5] = -15;
    }

    void update() override {
        if (state == 0) {
            // Input
            if (digitalRead(btnLeft) == LOW) pX -= 3.0;
            if (digitalRead(btnRight) == LOW) pX += 3.0;
            
            // Screen wrap
            if (pX < -8) pX = 128;
            if (pX > 128) pX = -8;
            
            // Physics
            pVY += gravity;
            pY += pVY;
            
            // Camera / Scrolling
            if (pY < 32) {
                float diff = 32 - pY;
                pY = 32; // Keep player in upper middle screen
                score += diff;
                
                for (int i = 0; i < 6; i++) {
                    platY[i] += diff;
                    
                    if (platY[i] > 64) {
                        // Find the highest platform to place the recycled one above it
                        float minY = 100;
                        for (int j = 0; j < 6; j++) {
                            if (platY[j] < minY) minY = platY[j];
                        }
                        
                        platX[i] = random(0, 108); // 128 - 20 (plat width)
                        platY[i] = minY - random(15, 35);
                    }
                }
            }
            
            // Collision (Only check if falling)
            if (pVY > 0) {
                for (int i = 0; i < 6; i++) {
                    float pBottom = pY + 8;
                    float pRight = pX + 8;
                    float pLeft = pX;
                    
                    float tLeft = platX[i];
                    float tRight = platX[i] + 20;
                    float tTop = platY[i];
                    
                    float prevBottom = pBottom - pVY;
                    
                    // Continuous collision detection
                    if (pRight > tLeft && pLeft < tRight) {
                        if (prevBottom <= tTop + 1.5 && pBottom >= tTop) {
                            pY = tTop - 8;
                            pVY = jumpForce;
                            break; 
                        }
                    }
                }
            }
            
            // Game Over when falling off screen
            if (pY > 64) {
                state = 1;
                delay(500);
            }
            
            // Draw
            display.clearDisplay();
            
            // Draw Platforms
            for (int i = 0; i < 6; i++) {
                display.drawRect((int)platX[i], (int)platY[i], 20, 3, WHITE);
                display.drawLine((int)platX[i]+1, (int)platY[i]+1, (int)platX[i]+18, (int)platY[i]+1, WHITE); 
            }
            
            // Draw Player
            display.fillRect((int)pX, (int)pY, 8, 8, WHITE);
            
            // Face details
            display.drawPixel((int)pX + 2, (int)pY + 2, BLACK); // left eye
            display.drawPixel((int)pX + 5, (int)pY + 2, BLACK); // right eye
            display.drawPixel((int)pX + 3, (int)pY + 5, BLACK); // mouth
            display.drawPixel((int)pX + 4, (int)pY + 5, BLACK);
            
            // Score
            display.setCursor(0, 0);
            display.print((int)(score / 10)); 
            
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("GAME OVER");
            display.setCursor(35, 35); display.print("Score: "); display.print((int)(score / 10));
            display.setCursor(20, 50); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(DoodleJumpGame)
