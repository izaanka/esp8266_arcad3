#pragma once

class IceSkippingGame : public ArcadeGame {
private:
    float sY, sVY;
    int skips;
    int state;
    bool lastSelect;
    bool graceWindow;

public:
    const char* getCategory() override { return "Sports"; }
    const char* getName() override { return "Ice Skipping"; }

    void init() override {
        sY = 20; sVY = 0; skips = 0; state = 0; lastSelect = true; graceWindow = false;
    }

    void update() override {
        if (state == 0) {
            bool cS = (digitalRead(btnSelect) == LOW);

            sVY += 0.2; // gravity
            sY += sVY;

            if (sY > 48) { // Hit water
                if (cS && !lastSelect) { // Perfect skip
                    sVY = -4.0 + (skips * 0.1); // gets faster/flatter
                    if (sVY > -2.0) sVY = -2.0;
                    sY = 48;
                    skips++;
                } else if (!graceWindow) { // Missed
                    graceWindow = true; // wait a few frames to see if they press late
                }
            }
            
            if (sY > 55) { // Sank
                state = 1; delay(500);
            }
            
            if (sY < 45) graceWindow = false;

            lastSelect = cS;
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("SANK!");
            display.setCursor(30, 35); display.print("Skips: "); display.print(skips);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
            return;
        }

        display.clearDisplay();
        
        // Water
        display.drawLine(0, 50, 128, 50, WHITE);
        
        // Stone
        display.fillCircle(64, (int)sY, 3, WHITE);
        
        display.setCursor(0, 0); display.print(skips);
        display.display();
        delay(15);
    }
};

REGISTER_GAME(IceSkippingGame)
