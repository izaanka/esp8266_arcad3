#pragma once

class GolfGame : public ArcadeGame {
private:
    float bx, by, bvx, bvy;
    float hx, hy;
    int state; // 0=angle, 1=power, 2=roll, 3=win
    float angle, power;
    int dir;
    int strokes;
    bool lastSelect;

    void loadHole() {
        bx = 20; by = 50;
        hx = random(80, 110); hy = random(10, 40);
        state = 0; dir = 1; angle = -PI/4; power = 0; lastSelect = true;
    }

public:
    const char* getCategory() override { return "Sports"; }
    const char* getName() override { return "Golf"; }

    void init() override { strokes = 0; loadHole(); }

    void update() override {
        bool cS = (digitalRead(btnSelect) == LOW);
        
        if (state == 0) {
            angle += dir * 0.05;
            if (angle > PI/2 || angle < -PI) dir = -dir;
            if (cS && !lastSelect) { state = 1; dir = 1; power = 1.0; delay(200); }
        } else if (state == 1) {
            power += dir * 0.2;
            if (power > 6.0 || power < 1.0) dir = -dir;
            if (cS && !lastSelect) {
                state = 2; strokes++;
                bvx = cos(angle) * power;
                bvy = sin(angle) * power;
            }
        } else if (state == 2) {
            bx += bvx; by += bvy;
            bvx *= 0.95; bvy *= 0.95; // friction
            
            // Walls
            if (bx < 5) { bx = 5; bvx = -bvx * 0.5; }
            if (bx > 123) { bx = 123; bvx = -bvx * 0.5; }
            if (by < 5) { by = 5; bvy = -bvy * 0.5; }
            if (by > 59) { by = 59; bvy = -bvy * 0.5; }

            // Hole
            float dx = bx - hx; float dy = by - hy;
            if (dx*dx + dy*dy < 16 && abs(bvx) < 2.0 && abs(bvy) < 2.0) {
                state = 3; delay(500);
            } else if (abs(bvx) < 0.1 && abs(bvy) < 0.1) {
                state = 0; angle = -PI/4;
            }
        } else if (state == 3) {
            display.clearDisplay();
            display.setCursor(40, 20); display.print("HOLE IN!");
            display.setCursor(40, 35); display.print("Strokes: "); display.print(strokes);
            if (cS && !lastSelect) { loadHole(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
            lastSelect = cS;
            return;
        }
        
        lastSelect = cS;

        display.clearDisplay();
        
        // Hole
        display.fillCircle((int)hx, (int)hy, 4, WHITE);
        display.drawCircle((int)hx, (int)hy, 2, BLACK); // visual contrast
        display.drawLine((int)hx, (int)hy, (int)hx, (int)hy-8, WHITE); // flag pole
        display.fillTriangle((int)hx, (int)hy-8, (int)hx+4, (int)hy-6, (int)hx, (int)hy-4, WHITE);

        // Ball
        display.fillCircle((int)bx, (int)by, 2, WHITE);

        // Aim / Power
        if (state == 0) {
            display.drawLine((int)bx, (int)by, (int)bx + cos(angle)*15, (int)by + sin(angle)*15, WHITE);
        } else if (state == 1) {
            display.drawLine((int)bx, (int)by, (int)bx + cos(angle)*power*5, (int)by + sin(angle)*power*5, WHITE);
        }

        display.setCursor(0,0); display.print("Strokes:"); display.print(strokes);
        display.display();
        delay(15);
    }
};

REGISTER_GAME(GolfGame)
