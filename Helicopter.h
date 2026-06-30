#pragma once

class HelicopterGame : public ArcadeGame {
private:
    float pY = 32;
    float pVY = 0;
    int score = 0;
    int state = 0;
    float speed = 2.0;
    
    float caveTop[17];
    float caveBot[17];
    float offset = 0;

public:
    const char* getName() override { return "Helicopter"; }

    void init() override {
        pY = 32; pVY = 0; score = 0; state = 0; speed = 2.0; offset = 0;
        for(int i=0; i<17; i++) {
            caveTop[i] = 10;
            caveBot[i] = 54;
        }
    }

    void update() override {
        if (state == 0) {
            bool cS = (digitalRead(btnSelect) == LOW);
            
            if (cS) {
                pVY -= 0.15; // Thrust
            } else {
                pVY += 0.15; // Gravity
            }
            
            // Terminal velocity
            if (pVY > 4.0) pVY = 4.0;
            if (pVY < -4.0) pVY = -4.0;
            
            pY += pVY;
            
            offset += speed;
            if (offset >= 8) {
                offset -= 8;
                score++;
                if (score % 100 == 0) speed += 0.2;
                
                for(int i=0; i<16; i++) {
                    caveTop[i] = caveTop[i+1];
                    caveBot[i] = caveBot[i+1];
                }
                
                float gap = 44 - min(30, score/20); // Gap gets smaller!
                float center = random(10 + gap/2, 64 - gap/2);
                
                caveTop[16] = center - gap/2;
                caveBot[16] = center + gap/2;
            }
            
            display.clearDisplay();
            
            for(int i=0; i<16; i++) {
                int x1 = i*8 - (int)offset;
                int x2 = (i+1)*8 - (int)offset;
                display.drawLine(x1, (int)caveTop[i], x2, (int)caveTop[i+1], WHITE); 
                display.drawLine(x1, (int)caveBot[i], x2, (int)caveBot[i+1], WHITE);
                
                // fill outer
                display.drawLine(x1, 0, x1, (int)caveTop[i], WHITE);
                display.drawLine(x1, 64, x1, (int)caveBot[i], WHITE);
            }
            
            // Draw Copter
            display.fillRect(20, (int)pY - 2, 10, 4, WHITE);
            display.drawLine(25, (int)pY - 4, 25, (int)pY - 2, WHITE);
            if ((millis()/50)%2 == 0) display.drawLine(20, (int)pY - 4, 30, (int)pY - 4, WHITE); // blades
            else display.drawLine(22, (int)pY - 4, 28, (int)pY - 4, WHITE);
            
            // Collision Detection
            int cIdx = (25 + (int)offset) / 8;
            float t = ((25 + (int)offset) % 8) / 8.0;
            float topY = caveTop[cIdx] * (1-t) + caveTop[cIdx+1] * t;
            float botY = caveBot[cIdx] * (1-t) + caveBot[cIdx+1] * t;
            
            if (pY - 4 <= topY || pY + 2 >= botY) {
                state = 1;
                delay(500);
            }
            
            display.setCursor(0, 0); display.print(score);
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("CRASHED!");
            display.setCursor(35, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(HelicopterGame)
