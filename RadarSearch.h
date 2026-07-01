#pragma once

class RadarSearchGame : public ArcadeGame {
private:
    float cx, cy;
    float tx, ty;
    float rTimer;
    int score;
    int state;

    void spawnTarget() {
        tx = random(10, 118); ty = random(10, 54);
    }

public:
    const char* getCategory() override { return "Puzzle"; }
    const char* getName() override { return "Radar Search"; }
    int getScore() override { return score; }

    void init() override {
        cx = 64; cy = 32; score = 0; state = 0; rTimer = 0;
        spawnTarget();
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) cx -= 2.0;
            if (digitalRead(btnRight) == LOW) cx += 2.0;
            
            // Auto sweep Y
            cy += sin(millis() / 300.0) * 2.0;
            if (cy < 0) cy = 0;
            if (cy > 64) cy = 64;

            float dx = cx - tx; float dy = cy - ty;
            float dist = sqrt(dx*dx + dy*dy);

            // Radar ping speed based on dist
            float pingSpeed = 0.5 + (100.0 / (dist + 1.0));
            rTimer += pingSpeed;

            if (digitalRead(btnSelect) == LOW) {
                if (dist < 10) { score++; spawnTarget(); delay(200); }
                else { state = 1; delay(500); } // Wrong guess
            }

            display.clearDisplay();
            
            // Crosshair
            display.drawLine(cx-4, cy, cx+4, cy, WHITE);
            display.drawLine(cx, cy-4, cx, cy+4, WHITE);
            
            // Radar ping visual
            if (rTimer > 20) {
                display.drawCircle((int)cx, (int)cy, (int)rTimer, WHITE);
                if (rTimer > 40) rTimer = 0;
            }

            display.setCursor(0,0); display.print("Score: "); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("WRONG SPOT!");
            display.setCursor(30, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(RadarSearchGame)
