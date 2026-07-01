#pragma once

class GeometryDashGame : public ArcadeGame {
private:
    float pY = 54, pVY = 0;
    const float gravity = 0.7;
    const float jumpForce = -6.0;
    bool isGrounded = true;
    
    float obsX = 128;
    int obsType = 0;
    int obsParam1 = 1; 
    int obsParam2 = 0; 
    
    float speed = 3.0; // horizontal speed
    
    int score = 0;
    int state = 500;
    bool lastSelect = true;

public:
    const char* getCategory() override { return "Arcade"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Geometry Dash"; }

    void init() override {
        pY = 54;
        pVY = 0;
        isGrounded = true;
        obsX = 128;
        obsType = 0;
        obsParam1 = 1;
        obsParam2 = 0;
        speed = 3.0;
        score = 0;
        state = 500;
        lastSelect = true;
    }

    void update() override {
        if (state == 500) {
            bool currSelect = digitalRead(btnSelect);
            if (currSelect == LOW && lastSelect == HIGH && isGrounded) {
                pVY = jumpForce;
                isGrounded = false;
            }
            lastSelect = currSelect;

            pVY += gravity;
            obsX -= speed;
            
            float currentFloorY = 54;
            int pLeft = 20;
            int pRight = 28;
            
            if (obsType == 1 || obsType == 2) {
                int tLeft = (int)obsX;
                int tRight = (int)obsX + obsParam1;
                
                if (pRight > tLeft && pLeft < tRight) {
                    if (obsType == 1) { // Tower
                        int tTop = 62 - obsParam2;
                        if (pY + 8 > tTop + 0.1) {
                            state = 501;
                            delay(500);
                        } else {
                            currentFloorY = tTop - 8;
                        }
                    } else if (obsType == 2) { // Platform
                        int tTop = 62 - obsParam2;
                        int tBottom = tTop + 6;
                        if (pY + 8 > tTop + 0.1 && pY < tBottom - 0.1) {
                            state = 501;
                            delay(500);
                        } else if (pY + 8 <= tTop + 0.1) {
                            currentFloorY = tTop - 8;
                        }
                    }
                }
            } else if (obsType == 3) { // Pit
                int tLeft = (int)obsX;
                int tRight = (int)obsX + obsParam1;
                if (pRight > tLeft + 2 && pLeft < tRight - 2) {
                    currentFloorY = 100; // bottomless pit
                }
            } else if (obsType == 4) { // Floating Spikes
                int tLeft = (int)obsX;
                int tRight = (int)obsX + (obsParam1 * 12);
                int tBottom = 62 - obsParam2; // tip of spike
                if (pRight > tLeft + 2 && pLeft < tRight - 2) {
                    if (pY < tBottom) {
                        state = 501; delay(500);
                    }
                }
            } else if (obsType == 5) { // Staircase
                int tLeft = (int)obsX;
                int stepWidth = obsParam2;
                int numSteps = obsParam1;
                int tRight = tLeft + stepWidth * numSteps;
                if (pRight > tLeft && pLeft < tRight) {
                    int stepIdx = (pRight - tLeft - 1) / stepWidth;
                    if (stepIdx < 0) stepIdx = 0;
                    if (stepIdx >= numSteps) stepIdx = numSteps - 1;
                    
                    int stepTop = 62 - (stepIdx + 1) * 8;
                    if (pY + 8 > stepTop + 0.1) {
                        state = 501; delay(500); // hit side
                    } else {
                        currentFloorY = stepTop - 8;
                    }
                }
            }
            
            pY += pVY;
            
            if (pY >= currentFloorY && state != 501) {
                pY = currentFloorY;
                pVY = 0;
                isGrounded = true;
            } else {
                isGrounded = false;
            }

            if (pY > 64 && state != 501) {
                state = 501;
                delay(500);
            }

            // Spike check
            if (obsType == 0 && state != 501) {
                int sLeft = (int)obsX + 2;
                int sRight = (int)obsX + (obsParam1 * 12) - 2;
                int sTop = 56;
                if (pRight > sLeft && pLeft < sRight && pY + 8 > sTop) {
                    state = 501;
                    delay(500);
                }
            }
            
            int obsWidth = (obsType == 0 || obsType == 4) ? (obsParam1 * 12) : ((obsType == 5) ? (obsParam1 * obsParam2) : obsParam1);
            if (obsX < -obsWidth && state != 501) {
                obsX = 128 + random(10, 30);
                obsType = random(0, 6); // 0: Spikes, 1: Tower, 2: Platform, 3: Pit, 4: Floating Spikes, 5: Staircase
                
                if (obsType == 0) {
                    obsParam1 = random(1, 4);
                } else if (obsType == 1) {
                    obsParam1 = random(16, 32);
                    obsParam2 = random(12, 22);
                } else if (obsType == 2) {
                    obsParam1 = random(20, 36);
                    obsParam2 = random(15, 25);
                } else if (obsType == 3) {
                    obsParam1 = random(25, 45); 
                } else if (obsType == 4) {
                    obsParam1 = random(1, 3); // num spikes
                    obsParam2 = random(15, 25); // height from ground to tip
                } else if (obsType == 5) {
                    obsParam1 = random(2, 4); // num steps
                    obsParam2 = random(8, 14); // step width
                }
                
                score++;
                if (score % 5 == 0 && speed < 7.0) speed += 0.4;
            }

            display.clearDisplay();
            
            // Draw Ground
            if (obsType == 3) {
                int pitStart = max(0, (int)obsX);
                int pitEnd = min(128, (int)obsX + obsParam1);
                if (pitStart > 0) display.drawLine(0, 62, pitStart, 62, WHITE);
                if (pitEnd < 128) display.drawLine(pitEnd, 62, 128, 62, WHITE);
            } else {
                display.drawLine(0, 62, 128, 62, WHITE);
            }
            
            // Draw Player Cube
            display.drawRect(pLeft, (int)pY, 8, 8, WHITE);
            display.fillRect(pLeft + 2, (int)pY + 2, 4, 4, WHITE);
            
            // Draw Obstacles
            if (obsType == 0) {
                for (int i = 0; i < obsParam1; i++) {
                    int sx = (int)obsX + (i * 12);
                    display.fillTriangle(sx, 62, sx + 6, 54, sx + 12, 62, WHITE);
                }
            } else if (obsType == 1) {
                display.drawRect((int)obsX, 62 - obsParam2, obsParam1, obsParam2, WHITE);
                if (obsParam1 > 4 && obsParam2 > 4) {
                    display.drawRect((int)obsX + 2, 62 - obsParam2 + 2, obsParam1 - 4, obsParam2 - 4, WHITE);
                }
            } else if (obsType == 2) {
                display.drawRect((int)obsX, 62 - obsParam2, obsParam1, 6, WHITE);
                display.drawLine((int)obsX + 2, 62 - obsParam2 + 2, (int)obsX + obsParam1 - 3, 62 - obsParam2 + 2, WHITE);
            } else if (obsType == 4) { // Floating Spikes
                for (int i = 0; i < obsParam1; i++) {
                    int sx = (int)obsX + (i * 12);
                    int sBot = 62 - obsParam2;
                    int sTop = sBot - 8;
                    display.fillTriangle(sx, sTop, sx + 6, sBot, sx + 12, sTop, WHITE);
                    display.fillRect(sx, sTop - 10, 12, 10, WHITE);
                }
            } else if (obsType == 5) { // Staircase
                for (int i = 0; i < obsParam1; i++) {
                    int sx = (int)obsX + i * obsParam2;
                    int h = (i + 1) * 8;
                    display.drawRect(sx, 62 - h, obsParam2, h, WHITE);
                }
            }

            display.setCursor(0, 0);
            display.print(score);
            
            display.display();
            delay(15);
        } else if (state == 501) {
            display.clearDisplay();
            display.setCursor(20, 20); display.print("CRASHED!");
            display.setCursor(20, 35); display.print("Score: "); display.print(score);
            display.setCursor(20, 50); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(GeometryDashGame)
