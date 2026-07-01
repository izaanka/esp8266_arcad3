#pragma once

class TargetShooterGame : public ArcadeGame {
private:
    float cx, cy;
    float tx, ty, tvx, tvy;
    bool tActive;
    int score;
    int ammo;
    int state;
    bool lastS;
    float timer;

    void spawnTarget() {
        tx = (random(2) == 0) ? -10 : 138;
        ty = random(40, 60);
        tvx = (tx < 0) ? random(20,40)*0.1 : -random(20,40)*0.1;
        tvy = -random(30, 50)*0.1;
        tActive = true;
    }

public:
    const char* getCategory() override { return "Action"; }
    const char* getName() override { return "Target Shoot"; }
    int getScore() override { return score; }

    void init() override {
        cx = 64; cy = 32; score = 0; ammo = 10; state = 0;
        tActive = false; lastS = true; timer = 100;
    }

    void update() override {
        if (state == 0) {
            timer -= 0.5;
            if (timer <= 0) { state = 1; delay(500); }

            bool cL = (digitalRead(btnLeft) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);

            // Move Crosshair
            if (cL) cx -= 2.0;
            if (cR) cx += 2.0;
            
            // Sweep Y automatically or just stay in upper half
            cy = 20 + sin(millis() / 500.0) * 10; 

            if (!tActive && random(100) < 5) spawnTarget();

            if (tActive) {
                tx += tvx; ty += tvy; tvy += 0.2; // gravity arc
                if (ty > 70) tActive = false; // missed
            }

            if (cS && !lastS && ammo > 0) {
                ammo--;
                if (tActive) {
                    float dx = cx - tx; float dy = cy - ty;
                    if (dx*dx + dy*dy < 100) { // Hit
                        tActive = false; score++; timer += 20; // +time
                        if (timer > 100) timer = 100;
                        if (score % 3 == 0) ammo += 3;
                    }
                }
            }
            lastS = cS;

            if (ammo <= 0 && !tActive) { state = 1; delay(500); }

            display.clearDisplay();
            
            // Crosshair
            display.drawLine(cx-5, cy, cx+5, cy, WHITE);
            display.drawLine(cx, cy-5, cx, cy+5, WHITE);

            // Target
            if (tActive) display.drawCircle((int)tx, (int)ty, 4, WHITE);

            display.setCursor(0,0); display.print("S:"); display.print(score);
            display.setCursor(45,0); display.print("A:"); display.print(ammo);
            display.setCursor(90,0); display.print("T:"); display.print((int)timer);
            
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(40, 20); display.print("TIME UP!");
            display.setCursor(40, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(TargetShooterGame)
