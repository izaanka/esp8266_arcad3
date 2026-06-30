#pragma once

class SubmarineGame : public ArcadeGame {
private:
    float pY = 32;
    int score = 0;
    int state = 0;
    float speed = 2.0;
    
    float caveTop[17];
    float caveBot[17];
    float offset = 0;
    
    float targetTop = 10, targetBot = 54;
    float currentTop = 10, currentBot = 54;

public:
    const char* getName() override { return "Submarine Cave"; }

    void init() override {
        pY = 32; score = 0; state = 0; speed = 2.0; offset = 0;
        targetTop = 10; targetBot = 54; currentTop = 10; currentBot = 54;
        for(int i=0; i<17; i++) {
            caveTop[i] = 10;
            caveBot[i] = 54;
        }
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) pY -= 1.5; 
            if (digitalRead(btnRight) == LOW) pY += 1.5; 
            
            offset += speed;
            
            if (offset >= 8) {
                offset -= 8;
                score++;
                if (score % 100 == 0 && speed < 5.0) speed += 0.2; 
                
                // Shift array left
                for(int i=0; i<16; i++) {
                    caveTop[i] = caveTop[i+1];
                    caveBot[i] = caveBot[i+1];
                }
                
                // Smoothly adjust target
                if (random(100) < 20) {
                    float gap = random(25, 45); 
                    if (score > 300) gap = random(20, 35);
                    
                    targetTop = random(5, 60 - gap);
                    targetBot = targetTop + gap;
                }
                
                currentTop += (targetTop - currentTop) * 0.2;
                currentBot += (targetBot - currentBot) * 0.2;
                
                // Add noise
                caveTop[16] = currentTop + random(-3, 3);
                caveBot[16] = currentBot + random(-3, 3);
            }
            
            display.clearDisplay();
            
            // Draw Cave
            for(int i=0; i<16; i++) {
                int x1 = i*8 - (int)offset;
                int x2 = (i+1)*8 - (int)offset;
                
                // Top Cave Fill
                display.drawLine(x1, 0, x1, (int)caveTop[i], WHITE);
                display.drawLine(x2, 0, x2, (int)caveTop[i+1], WHITE);
                display.drawLine(x1, (int)caveTop[i], x2, (int)caveTop[i+1], WHITE); // surface
                
                // Bottom Cave Fill
                display.drawLine(x1, 64, x1, (int)caveBot[i], WHITE);
                display.drawLine(x2, 64, x2, (int)caveBot[i+1], WHITE);
                display.drawLine(x1, (int)caveBot[i], x2, (int)caveBot[i+1], WHITE); // surface
            }
            
            // Draw Submarine
            display.fillRoundRect(28, (int)pY - 3, 12, 6, 2, WHITE);
            display.fillRect(32, (int)pY - 5, 4, 3, WHITE); // periscope
            display.drawPixel(26, (int)pY, WHITE); // propeller
            
            // Collision Detection
            int subIdx = (32 + (int)offset) / 8;
            float t = ((32 + (int)offset) % 8) / 8.0;
            float topY = caveTop[subIdx] * (1-t) + caveTop[subIdx+1] * t;
            float botY = caveBot[subIdx] * (1-t) + caveBot[subIdx+1] * t;
            
            if (pY - 4 <= topY || pY + 4 >= botY) {
                state = 1; // Crashed
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

REGISTER_GAME(SubmarineGame)
