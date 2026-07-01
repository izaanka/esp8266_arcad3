#pragma once

class JavelinThrowGame : public ArcadeGame {
private:
    float speed;
    float angle;
    float dist;
    int state; // 0=run, 1=angle, 2=fly, 3=result
    int lastBtn;
    float jx, jy, jvx, jvy;

public:
    const char* getCategory() override { return "Sports"; }
    const char* getName() override { return "Javelin Throw"; }

    void init() override {
        speed = 0; angle = 0; dist = 0; state = 0; lastBtn = -1;
    }

    void update() override {
        if (state == 0) { // Run
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);

            if (cL && lastBtn != 0) { speed += 0.8; lastBtn = 0; }
            if (cR && lastBtn != 1) { speed += 0.8; lastBtn = 1; }
            speed -= 0.1;
            if (speed < 0) speed = 0;
            
            if (cS) { state = 1; }
            
            display.clearDisplay();
            display.setCursor(20, 20); display.print("Mash L/R to run!");
            display.setCursor(20, 35); display.print("Hold Select to throw");
            display.fillRect(10, 50, (int)(speed * 5), 5, WHITE);
            display.display();
            delay(15);
        } else if (state == 1) { // Angle
            bool cS = (digitalRead(btnSelect) == LOW);
            angle += 0.05;
            if (angle > PI/2) angle = 0;

            if (!cS) { 
                state = 2; 
                jvx = speed * cos(angle);
                jvy = -speed * sin(angle);
                jx = 10; jy = 50;
            }

            display.clearDisplay();
            display.drawLine(10, 50, 10 + cos(angle)*20, 50 - sin(angle)*20, WHITE);
            display.setCursor(10, 10); display.print("Release to throw!");
            display.display();
            delay(15);
        } else if (state == 2) { // Fly
            jx += jvx; jy += jvy;
            jvy += 0.2; // gravity
            dist += jvx; // track total distance
            
            if (jy > 50) { state = 3; delay(500); }
            
            display.clearDisplay();
            display.drawLine(0, 50, 128, 50, WHITE);
            // Draw javelin fixed on screen horizontally if moving fast, or just a line
            int drawX = (jx < 64) ? (int)jx : 64;
            display.drawLine(drawX, (int)jy, drawX - 10, (int)jy - (jvy * 2), WHITE); // approximation of angle
            display.setCursor(0, 0); display.print("Dist: "); display.print((int)dist);
            display.display();
            delay(15);
        } else if (state == 3) { // Result
            display.clearDisplay();
            display.setCursor(30, 20); display.print("THROWN!");
            display.setCursor(30, 35); display.print("Dist: "); display.print((int)dist);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(JavelinThrowGame)
