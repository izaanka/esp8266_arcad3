#pragma once

class FallDownGame : public ArcadeGame {
private:
    float pX = 64, pY = 10;
    int score = 0;
    int state = 0;
    float speed = 1.0;
    
    struct Platform { float y; int gapX; };
    Platform plats[6];

public:
    const char* getName() override { return "Fall Down"; }

    void init() override {
        pX = 64; pY = 10; score = 0; state = 0; speed = 1.0;
        for(int i=0; i<6; i++) {
            plats[i].y = 20 + i*16;
            plats[i].gapX = random(10, 100);
        }
    }

    void update() override {
        if (state == 0) {
            // Player horizontal movement
            if (digitalRead(btnLeft) == LOW) pX -= 3.0;
            if (digitalRead(btnRight) == LOW) pX += 3.0;
            
            if (pX < 2) pX = 2;
            if (pX > 125) pX = 125;
            
            // Player vertical physics (falling)
            pY += 2.0; // fall speed
            
            // Move platforms UP
            for(int i=0; i<6; i++) {
                plats[i].y -= speed;
                
                if (plats[i].y < -5) {
                    // Recycle platform
                    plats[i].y = 90;
                    plats[i].gapX = random(10, 100);
                    score++;
                    if (score % 10 == 0) speed += 0.2;
                }
            }
            
            // Collision with platforms
            bool onPlat = false;
            for(int i=0; i<6; i++) {
                // platform is at plats[i].y, thickness is 2
                // gap is from gapX to gapX + 24
                if (pY + 3 > plats[i].y - 1 && pY - 3 < plats[i].y + 1) {
                    if (pX < plats[i].gapX || pX > plats[i].gapX + 24) {
                        pY = plats[i].y - 3; // Push player up
                        onPlat = true;
                    }
                }
            }
            
            // Game Over checks
            if (pY < 2) {
                state = 1; // squished at top
                delay(500);
            }
            if (pY > 64) pY = 64; // Can't fall past bottom of screen
            
            display.clearDisplay();
            
            // Draw Platforms
            for(int i=0; i<6; i++) {
                if (plats[i].y >= 0 && plats[i].y < 64) {
                    display.drawLine(0, (int)plats[i].y, plats[i].gapX, (int)plats[i].y, WHITE);
                    display.drawLine(plats[i].gapX + 24, (int)plats[i].y, 128, (int)plats[i].y, WHITE);
                }
            }
            
            // Draw Player
            display.fillCircle((int)pX, (int)pY, 3, WHITE);
            
            display.setCursor(0, 0); display.print(score);
            
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("SQUISHED!");
            display.setCursor(35, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(FallDownGame)
