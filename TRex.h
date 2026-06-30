#pragma once

class TRexGame : public ArcadeGame {
private:
    float pY = 50;
    float pVY = 0;
    bool ducking = false;
    
    struct Obstacle { float x; int type; }; // 0=cactus, 1=bird
    Obstacle obs[3];
    
    int score = 0;
    int state = 0;
    float speed = 3.0;

    void spawnObstacle(int i) {
        float max_x = 128;
        for(int j=0; j<3; j++) {
            if (obs[j].x > max_x) max_x = obs[j].x;
        }
        obs[i].x = max_x + random(60, 120);
        obs[i].type = (random(10) > 7) ? 1 : 0; // 30% bird
    }

public:
    const char* getName() override { return "T-Rex Run"; }

    void init() override {
        pY = 50; pVY = 0; ducking = false;
        score = 0; state = 0; speed = 3.0;
        obs[0].x = 128; obs[0].type = 0;
        spawnObstacle(1); spawnObstacle(2);
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW); // Duck
            bool cR = (digitalRead(btnRight) == LOW); // Jump
            
            ducking = (cL && pY >= 50); // can only duck on ground
            
            if (cR && pY >= 50) {
                pVY = -5.5; // Jump
            }
            
            pVY += 0.4; // Gravity
            pY += pVY;
            if (pY > 50) {
                pY = 50; pVY = 0;
            }
            
            score++;
            if (score % 200 == 0) speed += 0.3;
            
            for(int i=0; i<3; i++) {
                obs[i].x -= speed;
                if (obs[i].x < -10) {
                    spawnObstacle(i);
                }
                
                // Collision
                int pH = ducking ? 6 : 12;
                int pT = pY - pH;
                int pB = pY;
                int pL = 10;
                int pR = 18; // player x is 10 to 18
                
                int oL = obs[i].x;
                int oR = obs[i].x + 6;
                int oT = (obs[i].type == 0) ? 40 : 32; // cactus on ground (40 to 50), bird in air (32 to 38)
                int oB = (obs[i].type == 0) ? 50 : 38;
                
                if (pR > oL && pL < oR && pB > oT && pT < oB) {
                    state = 1;
                    delay(500);
                }
            }
            
            display.clearDisplay();
            
            // Ground
            display.drawLine(0, 50, 128, 50, WHITE);
            
            // Player
            if (ducking) {
                display.fillRect(10, 44, 12, 6, WHITE); // longer, shorter
            } else {
                display.fillRect(10, 38, 8, 12, WHITE); // tall
            }
            
            // Obstacles
            for(int i=0; i<3; i++) {
                if (obs[i].type == 0) {
                    display.fillRect((int)obs[i].x, 40, 6, 10, WHITE); // Cactus
                } else {
                    display.drawLine((int)obs[i].x, 35, (int)obs[i].x + 6, 35, WHITE); // Bird
                    if ((score / 10) % 2 == 0) {
                        display.drawLine((int)obs[i].x + 3, 35, (int)obs[i].x + 3, 30, WHITE); // wing up
                    } else {
                        display.drawLine((int)obs[i].x + 3, 35, (int)obs[i].x + 3, 40, WHITE); // wing down
                    }
                }
            }
            
            display.setCursor(80, 0); display.print(score/10);
            
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("CRASHED!");
            display.setCursor(35, 35); display.print("Score: "); display.print(score/10);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(TRexGame)
