#pragma once

class JugglingGame : public ArcadeGame {
private:
    float hx;
    struct Ball { float x, y, vx, vy; };
    Ball balls[3];
    int score;
    int state;

public:
    const char* getCategory() override { return "Sports"; }
    const char* getName() override { return "Juggling"; }
    int getScore() override { return score; }

    void init() override {
        hx = 64; score = 0; state = 0;
        for(int i=0; i<3; i++) {
            balls[i].x = 64 + (i-1)*20;
            balls[i].y = 30 + i*10;
            balls[i].vx = (i-1) * 0.5;
            balls[i].vy = -2.0;
        }
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) hx -= 2.5;
            if (digitalRead(btnRight) == LOW) hx += 2.5;
            
            for(int i=0; i<3; i++) {
                balls[i].x += balls[i].vx;
                balls[i].y += balls[i].vy;
                balls[i].vy += 0.1; // gravity

                if (balls[i].x < 0 || balls[i].x > 128) balls[i].vx = -balls[i].vx;

                if (balls[i].y > 56) {
                    if (abs(balls[i].x - hx) < 12) {
                        balls[i].y = 56;
                        balls[i].vy = -random(30, 50)*0.1;
                        balls[i].vx = (balls[i].x - hx) * 0.2;
                        score++;
                    } else {
                        state = 1; delay(500); // Dropped
                    }
                }
            }

            display.clearDisplay();
            
            // Hand
            display.drawLine((int)hx - 10, 58, (int)hx + 10, 58, WHITE);
            
            // Balls
            for(int i=0; i<3; i++) display.fillCircle((int)balls[i].x, (int)balls[i].y, 3, WHITE);

            display.setCursor(0,0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(40, 20); display.print("DROPPED!");
            display.setCursor(40, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(JugglingGame)
