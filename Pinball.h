#pragma once

class PinballGame : public ArcadeGame {
private:
    float bX, bY, bVX, bVY;
    int score = 0;
    int state = 600; // 600 = playing, 601 = game over
    
    int bumpX[5] = {50, 78, 64, 42, 86};
    int bumpY[5] = {20, 20, 35, 12, 12};
    
    bool lFlip = false;
    bool rFlip = false;
    bool lastLFlip = false;
    bool lastRFlip = false;

public:
    const char* getCategory() override { return "Arcade"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Pinball"; }

    void init() override {
        bX = random(40, 88);
        bY = 5;
        bVX = random(-10, 10) / 10.0;
        bVY = 0;
        score = 0;
        state = 600;
    }

    void update() override {
        if (state == 600) {
            lastLFlip = lFlip;
            lastRFlip = rFlip;
            lFlip = (digitalRead(btnLeft) == LOW);
            rFlip = (digitalRead(btnRight) == LOW);

            // Apply gravity
            bVY += 0.12; 
            
            // Limit speed
            if (bVY > 5.0) bVY = 5.0;
            if (bVX > 5.0) bVX = 5.0;
            if (bVX < -5.0) bVX = -5.0;

            bX += bVX;
            bY += bVY;

            // Wall Collisions
            if (bX < 34) { bX = 34; bVX = -bVX * 0.8; }
            if (bX > 94) { bX = 94; bVX = -bVX * 0.8; }
            if (bY < 2)  { bY = 2;  bVY = -bVY * 0.8; }

            // Sloped walls on bottom sides
            if (bY > 40) {
                // Left slope
                float slopeLeftX = 32 + ((bY - 40) / 20.0) * 13; // from 32 to 45
                if (bX < slopeLeftX) {
                    bX = slopeLeftX + 1;
                    bVX = 1.5;
                }
                
                // Right slope
                float slopeRightX = 96 - ((bY - 40) / 20.0) * 13; // from 96 to 83
                if (bX > slopeRightX) {
                    bX = slopeRightX - 1;
                    bVX = -1.5;
                }
            }

            // Bumper Collisions
            for(int i = 0; i < 5; i++) {
                float dx = bX - bumpX[i];
                float dy = bY - bumpY[i];
                float dist = sqrt(dx*dx + dy*dy);
                if (dist < 8) { 
                    score += 10;
                    // Anti-stick ejection
                    float nx = dx / dist;
                    float ny = dy / dist;
                    bX = bumpX[i] + nx * 9;
                    bY = bumpY[i] + ny * 9;
                    bVX = nx * 3.0;
                    bVY = ny * 3.0;
                }
            }

            // Top Curve Push
            if (bY < 15) {
                if (bX < 45) { bVX += 0.2; bVY += 0.1; }
                else if (bX > 83) { bVX -= 0.2; bVY += 0.1; }
            }

            // Slingshots
            if (bX > 40 && bX < 47 && bY > 48 && bY < 58) {
                bVX = 4.0; bVY = -3.0; score += 5; bX = 47;
            }
            if (bX > 81 && bX < 88 && bY > 48 && bY < 58) {
                bVX = -4.0; bVY = -3.0; score += 5; bX = 81;
            }

            // Left Flipper Zone (X: 45 to 60)
            if (bX >= 45 && bX <= 60) {
                float flipY = lFlip ? 60 - (bX - 45) * (8.0/15.0) : 60 + (bX - 45) * (4.0/15.0);
                if (bY >= flipY - 1 && bY <= flipY + 4) {
                    bY = flipY - 2;
                    if (lFlip && !lastLFlip) {
                        bVY = -5.0; 
                        bVX = (bX - 45) * 0.2 + 0.5;
                    } else if (lFlip) {
                        bVY = -bVY * 0.2;
                        bVX += 0.05;
                    } else {
                        bVY = -bVY * 0.2;
                        bVX += 0.2; // rolls right towards gap
                    }
                }
            }
            
            // Right Flipper Zone (X: 68 to 83)
            if (bX >= 68 && bX <= 83) {
                float flipY = rFlip ? 60 - (83 - bX) * (8.0/15.0) : 60 + (83 - bX) * (4.0/15.0);
                if (bY >= flipY - 1 && bY <= flipY + 4) {
                    bY = flipY - 2;
                    if (rFlip && !lastRFlip) {
                        bVY = -5.0;
                        bVX = (bX - 83) * 0.2 - 0.5;
                    } else if (rFlip) {
                        bVY = -bVY * 0.2;
                        bVX -= 0.05;
                    } else {
                        bVY = -bVY * 0.2;
                        bVX -= 0.2; // rolls left towards gap
                    }
                }
            }

            // Fall out condition
            if (bY > 64) {
                state = 601;
                delay(500);
            }

            // Drawing Phase
            display.clearDisplay();
            
            // Draw Walls
            display.drawLine(32, 10, 32, 40, WHITE);
            display.drawLine(32, 40, 45, 60, WHITE); // left slant
            
            display.drawLine(96, 10, 96, 40, WHITE);
            display.drawLine(96, 40, 83, 60, WHITE); // right slant
            
            // Top curve
            display.drawLine(32, 10, 42, 2, WHITE);
            display.drawLine(96, 10, 86, 2, WHITE);
            display.drawLine(42, 2, 86, 2, WHITE);

            // Slingshots
            display.drawLine(42, 50, 45, 58, WHITE);
            display.drawLine(86, 50, 83, 58, WHITE);
            
            // Draw Flippers
            if (lFlip) {
                display.drawLine(45, 60, 60, 52, WHITE);
                display.drawLine(45, 61, 60, 53, WHITE); // thickness
            } else {
                display.drawLine(45, 60, 60, 64, WHITE);
                display.drawLine(45, 61, 60, 65, WHITE);
            }
            
            if (rFlip) {
                display.drawLine(83, 60, 68, 52, WHITE);
                display.drawLine(83, 61, 68, 53, WHITE);
            } else {
                display.drawLine(83, 60, 68, 64, WHITE);
                display.drawLine(83, 61, 68, 65, WHITE);
            }
            
            // Draw Bumpers
            for(int i = 0; i < 5; i++) {
                display.drawCircle(bumpX[i], bumpY[i], 6, WHITE);
                display.fillCircle(bumpX[i], bumpY[i], 2, WHITE);
            }
            
            // Draw Ball
            display.fillCircle(bX, bY, 2, WHITE);
            
            // Draw Score
            display.setCursor(0, 0);
            display.print(score);

            display.display();
            delay(15);
            
        } else if (state == 601) {
            display.clearDisplay();
            display.setCursor(20, 20); display.print("GAME OVER");
            display.setCursor(20, 35); display.print("Score: "); display.print(score);
            display.setCursor(20, 50); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(PinballGame)
