#pragma once

class TronGame : public ArcadeGame {
private:
    float p1X, p1Y, p2X, p2Y;
    int dir1, dir2; // 0=up, 1=right, 2=down, 3=left
    int score1 = 0, score2 = 0;
    int state = 0;
    bool firstFrame = true;
    bool lastL = true, lastR = true;

    void resetRound() {
        p1X = 10; p1Y = 32; dir1 = 1;
        p2X = 118; p2Y = 32; dir2 = 3;
        firstFrame = true;
    }

public:
    const char* getName() override { return "Tron Lightcycles"; }

    void init() override {
        score1 = 0; score2 = 0; state = 0;
        resetRound();
        lastL = true; lastR = true;
    }

    void update() override {
        if (state == 0) {
            if (firstFrame) {
                display.clearDisplay();
                display.drawRect(0, 8, 128, 56, WHITE); // Borders
                display.display();
                firstFrame = false;
                delay(500); 
            }
            
            // Player Input
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);
            if (cL && !lastL) dir1 = (dir1 + 3) % 4; // Turn Left (relative)
            if (cR && !lastR) dir1 = (dir1 + 1) % 4; // Turn Right (relative)
            lastL = cL; lastR = cR;
            
            // AI Input (Simple avoid walls)
            int nextX = p2X, nextY = p2Y;
            if (dir2 == 0) nextY -= 2; else if (dir2 == 1) nextX += 2;
            else if (dir2 == 2) nextY += 2; else nextX -= 2;
            
            if (display.getPixel(nextX, nextY)) {
                if (random(2) == 0) dir2 = (dir2 + 3) % 4;
                else dir2 = (dir2 + 1) % 4;
            }
            
            // Move
            if (dir1 == 0) p1Y--; else if (dir1 == 1) p1X++; else if (dir1 == 2) p1Y++; else p1X--;
            if (dir2 == 0) p2Y--; else if (dir2 == 1) p2X++; else if (dir2 == 2) p2Y++; else p2X--;
            
            // Collision Detection using screen buffer!
            bool crash1 = display.getPixel((int)p1X, (int)p1Y);
            bool crash2 = display.getPixel((int)p2X, (int)p2Y);
            
            // Draw new positions
            display.drawPixel((int)p1X, (int)p1Y, WHITE);
            display.drawPixel((int)p2X, (int)p2Y, WHITE);
            
            // Draw Score without clearing the whole screen
            display.fillRect(0, 0, 128, 8, BLACK); 
            display.setCursor(10, 0); display.print("P1: "); display.print(score1);
            display.setCursor(80, 0); display.print("AI: "); display.print(score2);
            
            display.display();
            
            if (crash1 || crash2) {
                if (crash1 && !crash2) score2++;
                else if (crash2 && !crash1) score1++;
                
                if (score1 >= 3 || score2 >= 3) {
                    state = 1;
                } else {
                    resetRound();
                }
                delay(1000);
            } else {
                delay(20);
            }
            
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); 
            if (score1 > score2) display.print("YOU WIN!"); else display.print("AI WINS!");
            display.setCursor(20, 50); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(TronGame)
