#pragma once

class WhackAThiefGame : public ArcadeGame {
private:
    int windows[3]; // 0=empty, 1=thief, 2=innocent
    int timers[3];
    int score;
    int lives;
    int state;
    bool lastL, lastS, lastR;

public:
    const char* getCategory() override { return "Action"; }
    const char* getName() override { return "Whack Thief"; }
    int getScore() override { return score; }

    void init() override {
        for(int i=0; i<3; i++) { windows[i] = 0; timers[i] = 0; }
        score = 0; lives = 3; state = 0;
        lastL = true; lastS = true; lastR = true;
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);

            // Spawn
            if (random(100) < 4 + (score/10)) {
                int w = random(3);
                if (windows[w] == 0) {
                    windows[w] = (random(100) < 70) ? 1 : 2; // 70% thief
                    timers[w] = 50 - (score/2);
                    if (timers[w] < 15) timers[w] = 15;
                }
            }

            // Timeouts
            for(int i=0; i<3; i++) {
                if (windows[i] != 0) {
                    timers[i]--;
                    if (timers[i] <= 0) {
                        if (windows[i] == 1) { // missed thief
                            lives--;
                            if (lives <= 0) { state = 1; delay(500); }
                        }
                        windows[i] = 0;
                    }
                }
            }

            // Input
            if (cL && !lastL) {
                if (windows[0] == 1) { score++; windows[0] = 0; }
                else if (windows[0] == 2) { lives--; windows[0] = 0; if (lives<=0) state=1; }
            }
            if (cS && !lastS) {
                if (windows[1] == 1) { score++; windows[1] = 0; }
                else if (windows[1] == 2) { lives--; windows[1] = 0; if (lives<=0) state=1; }
            }
            if (cR && !lastR) {
                if (windows[2] == 1) { score++; windows[2] = 0; }
                else if (windows[2] == 2) { lives--; windows[2] = 0; if (lives<=0) state=1; }
            }

            lastL = cL; lastS = cS; lastR = cR;

            display.clearDisplay();
            
            // Draw Windows
            for(int i=0; i<3; i++) {
                int x = 20 + i*40;
                display.drawRect(x, 20, 20, 30, WHITE);
                if (windows[i] == 1) { // Thief (mask)
                    display.fillCircle(x+10, 30, 6, WHITE);
                    display.fillRect(x+4, 28, 12, 4, BLACK); // mask
                } else if (windows[i] == 2) { // Innocent
                    display.drawCircle(x+10, 30, 6, WHITE);
                    display.fillCircle(x+7, 28, 1, WHITE);
                    display.fillCircle(x+13, 28, 1, WHITE);
                }
            }

            display.setCursor(0, 0); display.print("S:"); display.print(score);
            display.setCursor(100, 0); display.print("L:"); display.print(lives);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("GAME OVER!");
            display.setCursor(30, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(WhackAThiefGame)
