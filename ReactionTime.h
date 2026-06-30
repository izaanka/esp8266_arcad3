#pragma once

class ReactionTimeGame : public ArcadeGame {
private:
    int state = 300; // 300=Menu, 301=Waiting, 302=React, 303=Result, 304=FalseStart
    unsigned long waitStart = 0;
    unsigned long waitDuration = 0;
    unsigned long reactStart = 0;
    unsigned long reactionTime = 0;
    bool lastSelect = true;
    bool timerStarted = false;

public:
    const char* getName() override { return "Reaction Time"; }

    void init() override {
        state = 300;
        lastSelect = true;
    }

    void update() override {
        bool currSelect = digitalRead(btnSelect);
        bool pressed = (currSelect == LOW && lastSelect == HIGH);
        lastSelect = currSelect;

        if (state == 300) {
            display.clearDisplay();
            display.setCursor(15, 20); display.print("REACTION TEST");
            display.setCursor(10, 40); display.print("Press D6 to Start");
            if (pressed) {
                state = 301;
                waitStart = millis();
                waitDuration = random(2000, 6000);
            }
        } 
        else if (state == 301) {
            display.clearDisplay();
            display.setCursor(30, 25); display.print("Wait for it...");
            
            if (pressed) {
                state = 304; // False start
                delay(500);
            } else if (millis() - waitStart > waitDuration) {
                state = 302;
                timerStarted = false;
            }
        }
        else if (state == 302) {
            display.clearDisplay();
            display.fillRect(0, 0, 128, 64, WHITE);
            display.setTextColor(BLACK);
            display.setCursor(30, 25); display.print("PRESS NOW!!!");
            display.setTextColor(WHITE);
            
            // Draw first, then start the timer on the very frame it becomes visible!
            if (!timerStarted) {
                display.display(); // Force display right now
                reactStart = millis();
                timerStarted = true;
            }
            
            if (pressed) {
                reactionTime = millis() - reactStart;
                state = 303;
                // No delay here, so the screen transitions to result immediately!
            }
        }
        else if (state == 303) {
            display.clearDisplay();
            display.setCursor(20, 10); display.print("YOUR TIME:");
            display.setCursor(20, 25); display.print(reactionTime); display.print(" ms");
            display.setCursor(10, 45); display.print("D6: Try Again");
            
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            if (pressed) {
                state = 301;
                waitStart = millis();
                waitDuration = random(2000, 6000);
            }
        }
        else if (state == 304) {
            display.clearDisplay();
            display.setCursor(20, 20); display.print("TOO EARLY!");
            display.setCursor(10, 40); display.print("D6: Try Again");
            
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            if (pressed) {
                state = 301;
                waitStart = millis();
                waitDuration = random(2000, 6000);
            }
        }

        display.display();
        delay(15);
    }
};

REGISTER_GAME(ReactionTimeGame)
