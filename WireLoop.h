#pragma once

class WireLoopGame : public ArcadeGame {
private:
    float px;
    float pathX[64]; // center of wire for each Y
    float curve;
    float speed;
    int score;
    int state;

    void advancePath() {
        for(int i=63; i>0; i--) {
            pathX[i] = pathX[i-1];
        }
        pathX[0] = pathX[1] + curve;
        if (pathX[0] < 20 || pathX[0] > 108) curve = -curve;
        if (random(100) < 5) curve += random(-1, 2) * 0.5;
        if (curve > 2.0) curve = 2.0;
        if (curve < -2.0) curve = -2.0;
    }

public:
    const char* getCategory() override { return "Puzzle"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Wire Loop"; }

    void init() override {
        px = 64; curve = 0.5; score = 0; state = 0; speed = 1.0;
        for(int i=0; i<64; i++) pathX[i] = 64;
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) px -= 1.5;
            if (digitalRead(btnRight) == LOW) px += 1.5;

            advancePath();
            score++;
            if (score % 200 == 0) speed += 0.5; // Actually advancePath multiple times if speed > 1
            if (speed > 1.5) {
                if (random(10) < 5) advancePath();
            }

            // Check collision at Y=50
            float wireCx = pathX[50];
            if (px < wireCx - 6 || px > wireCx + 6) {
                state = 1; delay(500);
            }

            display.clearDisplay();

            for(int i=0; i<63; i+=2) {
                display.drawLine((int)pathX[i], i, (int)pathX[i+2], i+2, WHITE);
            }

            display.drawCircle((int)px, 50, 4, WHITE);
            
            display.setCursor(0,0); display.print(score/10);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(40, 20); display.print("BZZZT!");
            display.setCursor(40, 35); display.print("Score: "); display.print(score/10);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(WireLoopGame)
