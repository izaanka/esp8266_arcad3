#pragma once

class BalloonPopGame : public ArcadeGame {
private:
    float size;
    int state;
    int score;
    float popLimit;

public:
    const char* getCategory() override { return "Action"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Balloon Pop"; }

    void init() override {
        size = 5; state = 0; score = 0; popLimit = random(20, 55);
    }

    void update() override {
        if (state == 0) {
            bool cS = (digitalRead(btnSelect) == LOW);
            if (cS) {
                size += 0.3;
                if (size > popLimit) {
                    state = 1; // Popped
                    delay(500);
                }
            } else if (size > 5.5) { // they let go after pumping
                state = 2; // Scored
                score = (int)size;
                delay(500);
            }
        } else {
            display.clearDisplay();
            if (state == 1) {
                display.setCursor(40, 20); display.print("POPPED!");
                display.setCursor(40, 35); display.print("Score: 0");
            } else {
                display.setCursor(40, 20); display.print("SAVED!");
                display.setCursor(40, 35); display.print("Score: "); display.print(score);
            }
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
            return;
        }

        display.clearDisplay();
        display.setCursor(0,0); display.print("Hold Select!");
        
        display.drawCircle(64, 40, (int)size, WHITE);
        display.drawLine(64, 40+(int)size, 64, 60, WHITE); // string

        display.display();
        delay(15);
    }
};

REGISTER_GAME(BalloonPopGame)
