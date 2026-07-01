#pragma once

class BlobVolleyGame : public ArcadeGame {
private:
    float bx, by, bvx, bvy;
    float px, py;
    float ax, ay;
    bool pJump, aJump;
    float pvy, avy;
    int pScore, aScore;
    int state;
    bool lastS;

    void resetBall(bool toPlayer) {
        bx = toPlayer ? 30 : 98;
        by = 20; bvx = 0; bvy = 0;
        px = 30; py = 60;
        ax = 98; ay = 60;
        pJump = false; aJump = false;
        state = 0;
    }

public:
    const char* getCategory() override { return "Sports"; }
    const char* getName() override { return "Blob Volley"; }
    int getScore() override { return pScore; }

    void init() override {
        pScore = 0; aScore = 0;
        resetBall(true);
        lastS = true;
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);

            // Player move
            if (cL) px -= 1.5;
            if (cR) px += 1.5;
            if (px < 10) px = 10;
            if (px > 58) px = 58;

            if (cS && !lastS && !pJump) { pJump = true; pvy = -4.0; }
            if (pJump) { py += pvy; pvy += 0.3; if(py >= 60) { py = 60; pJump = false; } }
            
            lastS = cS;

            // AI Move
            if (bx > 64) {
                if (ax < bx - 5) ax += 1.5;
                if (ax > bx + 5) ax -= 1.5;
                if (by > 30 && by < 40 && !aJump) { aJump = true; avy = -4.0; }
            } else {
                if (ax < 98) ax += 1.0;
                if (ax > 98) ax -= 1.0;
            }
            if (ax < 70) ax = 70;
            if (ax > 118) ax = 118;
            if (aJump) { ay += avy; avy += 0.3; if(ay >= 60) { ay = 60; aJump = false; } }

            // Ball Physics
            bx += bvx; by += bvy; bvy += 0.15; // gravity
            
            // Walls
            if (bx < 5) { bx = 5; bvx = -bvx; }
            if (bx > 123) { bx = 123; bvx = -bvx; }
            
            // Net
            if (bx > 62 && bx < 66 && by > 40) {
                if (bx < 64) { bx = 62; bvx = -bvx; }
                else { bx = 66; bvx = -bvx; }
            }

            // Player collision (semi-circle approximation)
            float dx = bx - px; float dy = by - py;
            if (dx*dx + dy*dy < 100 && dy < 0) {
                bvy = -4.0; bvx = dx * 0.3;
            }
            // AI collision
            dx = bx - ax; dy = by - ay;
            if (dx*dx + dy*dy < 100 && dy < 0) {
                bvy = -4.0; bvx = dx * 0.3;
            }

            // Ground
            if (by > 60) {
                if (bx < 64) { aScore++; resetBall(true); } // AI scored
                else { pScore++; resetBall(false); } // Player scored
                
                if (pScore >= 5) state = 1;
                if (aScore >= 5) state = 2;
            }

            display.clearDisplay();
            // Net
            display.drawLine(64, 64, 64, 45, WHITE);
            // Ball
            display.fillCircle((int)bx, (int)by, 3, WHITE);
            // Player Blob
            display.fillCircle((int)px, (int)py, 8, WHITE);
            display.fillRect((int)px - 8, (int)py, 17, 9, BLACK); // cut off bottom
            // AI Blob
            display.fillCircle((int)ax, (int)ay, 8, WHITE);
            display.fillRect((int)ax - 8, (int)ay, 17, 9, BLACK);

            display.setCursor(10, 0); display.print(pScore);
            display.setCursor(110, 0); display.print(aScore);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); 
            if (state == 1) display.print("YOU WON!");
            else display.print("AI WON!");
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(BlobVolleyGame)
