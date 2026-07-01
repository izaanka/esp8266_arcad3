#pragma once

class ZombieSurvivalGame : public ArcadeGame {
private:
    struct Zombie { float x; bool active; };
    Zombie zombies[4];
    int score;
    int state;
    bool lastL, lastR;
    float spawnRate;

public:
    const char* getCategory() override { return "Action"; }
    const char* getName() override { return "Zombie Survive"; }
    int getScore() override { return score; }

    void init() override {
        score = 0; state = 0; spawnRate = 2.0;
        lastL = true; lastR = true;
        for(int i=0; i<4; i++) zombies[i].active = false;
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);
            
            // Attack animation timer
            static int attL = 0; static int attR = 0;
            if (attL > 0) attL--;
            if (attR > 0) attR--;

            // Spawn
            if (random(100) < spawnRate) {
                for(int i=0; i<4; i++) {
                    if (!zombies[i].active) {
                        zombies[i].active = true;
                        zombies[i].x = (random(2) == 0) ? -10 : 138;
                        break;
                    }
                }
            }

            // Move and Check Attack
            for(int i=0; i<4; i++) {
                if (zombies[i].active) {
                    if (zombies[i].x < 64) zombies[i].x += 0.5 + (score*0.01);
                    else zombies[i].x -= 0.5 + (score*0.01);
                    
                    // Player attack
                    if (cL && !lastL && zombies[i].x > 40 && zombies[i].x < 64) {
                        zombies[i].active = false; score++; attL = 5;
                    }
                    if (cR && !lastR && zombies[i].x < 88 && zombies[i].x > 64) {
                        zombies[i].active = false; score++; attR = 5;
                    }

                    // Game over check
                    if (abs(zombies[i].x - 64) < 6) {
                        state = 1; delay(500);
                    }
                }
            }

            lastL = cL; lastR = cR;

            display.clearDisplay();
            // Ground
            display.drawLine(0, 50, 128, 50, WHITE);
            
            // Player
            display.fillRect(60, 40, 8, 10, WHITE);
            if (attL > 0) display.drawLine(60, 45, 45, 45, WHITE); // gun L
            if (attR > 0) display.drawLine(68, 45, 83, 45, WHITE); // gun R

            // Zombies
            for(int i=0; i<4; i++) {
                if (zombies[i].active) {
                    display.drawRect((int)zombies[i].x - 4, 42, 8, 8, WHITE);
                    display.drawLine((int)zombies[i].x, 42, (int)zombies[i].x + (zombies[i].x < 64 ? 4 : -4), 45, WHITE); // zombie arms
                }
            }
            
            display.setCursor(0, 0); display.print("Score: "); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(40, 20); display.print("BITTEN!");
            display.setCursor(40, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(ZombieSurvivalGame)
