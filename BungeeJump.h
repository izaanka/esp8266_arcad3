#pragma once

class BungeeJumpGame : public ArcadeGame {
private:
    float py, pvy;
    float cordLen;
    float maxDrop;
    int state; // 0=falling, 1=bouncing, 2=result
    int score;

public:
    const char* getCategory() override { return "Action"; }
    const char* getName() override { return "Bungee Jump"; }
    int getScore() override { return score; }

    void init() override {
        py = 0; pvy = 0; cordLen = -1; maxDrop = 0; state = 0; score = 0;
    }

    void loadLevel() {
        py = 0; pvy = 0; cordLen = -1; maxDrop = 0; state = 0;
    }

    void update() override {
        if (state == 0) {
            bool cS = (digitalRead(btnSelect) == LOW);

            py += pvy; pvy += 0.2; // gravity

            if (cS && cordLen == -1) {
                cordLen = py; // engaged bungee
                state = 1;
            }
            
            if (py > 250) { // Splat! Floor is at 250 logic
                score = 0; state = 2; delay(500);
            }

            display.clearDisplay();
            display.drawLine(0, 60, 128, 60, WHITE); // floor
            
            // Draw player relative to floor
            int drawY = 60 - (250 - (int)py); 
            if (drawY < 0) drawY = 0;

            display.fillRect(60, drawY - 8, 8, 8, WHITE);
            
            display.setCursor(0,0); display.print("Height: "); display.print(250 - (int)py);
            display.display();
            delay(15);
        } else if (state == 1) { // Bouncing
            py += pvy; pvy += 0.2; // gravity
            
            if (py > maxDrop) maxDrop = py;

            if (py > cordLen) {
                pvy -= (py - cordLen) * 0.02; // spring force
            }

            pvy *= 0.99; // dampening

            if (py > 250) { score = 0; state = 2; delay(500); } // Hit floor
            
            if (abs(pvy) < 0.2 && py < cordLen + 10) { // come to rest
                score += (int)maxDrop; state = 2; delay(500);
            }

            display.clearDisplay();
            display.drawLine(0, 60, 128, 60, WHITE); // floor
            
            int drawY = 60 - (250 - (int)py); 
            if (drawY < 0) drawY = 0;
            
            display.fillRect(60, drawY - 8, 8, 8, WHITE);
            display.drawLine(64, 0, 64, drawY - 8, WHITE); // cord

            display.display();
            delay(15);
        } else if (state == 2) {
            display.clearDisplay();
            display.setCursor(40, 20); 
            if (score > 0) display.print("SAFE!"); else display.print("SPLAT!");
            display.setCursor(40, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { if(score>0) loadLevel(); else init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(BungeeJumpGame)
