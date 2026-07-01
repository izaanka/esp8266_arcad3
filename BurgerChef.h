#pragma once

class BurgerChefGame : public ArcadeGame {
private:
    float px;
    float ix, iy; // ingredient
    int iType; // 0=botBun, 1=meat, 2=cheese, 3=topBun, 4=bad
    int stack; // current required next: 0, 1, 2, 3
    int score;
    int state;
    float speed;

    void spawnIngredient() {
        ix = random(20, 108); iy = 0;
        // Bias towards what we need, but sometimes bad
        if (random(100) < 40) iType = stack;
        else iType = random(5);
    }

public:
    const char* getCategory() override { return "Action"; }
    const char* getName() override { return "Burger Chef"; }
    int getScore() override { return score; }

    void init() override {
        px = 64; stack = 0; score = 0; state = 0; speed = 1.5;
        spawnIngredient();
    }

    void drawIngredient(int type, int x, int y) {
        if (type == 0 || type == 3) { // Bun
            display.fillCircle(x, y+2, 6, WHITE);
            display.fillRect(x-6, y+2, 13, 6, BLACK);
        } else if (type == 1) { // Meat
            display.fillRect(x-7, y, 14, 4, WHITE);
        } else if (type == 2) { // Cheese
            display.drawLine(x-6, y, x+6, y, WHITE);
        } else if (type == 4) { // Bad (Fish bone)
            display.drawLine(x-4, y, x+4, y, WHITE);
            display.drawLine(x, y-4, x, y+4, WHITE);
        }
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) px -= 2.0;
            if (digitalRead(btnRight) == LOW) px += 2.0;

            iy += speed;
            if (iy > 60) spawnIngredient(); // missed

            if (iy > 50 && abs(ix - px) < 10) { // Caught
                if (iType == stack) {
                    stack++;
                    if (stack > 3) { // completed burger
                        score++; stack = 0; speed += 0.1;
                    }
                    spawnIngredient();
                } else {
                    state = 1; delay(500); // wrong ingredient
                }
            }

            display.clearDisplay();
            
            // Draw Player plate
            display.drawLine((int)px - 10, 56, (int)px + 10, 56, WHITE);
            
            // Draw current stack
            for(int i=0; i<stack; i++) {
                drawIngredient(i, (int)px, 54 - (i*4));
            }

            // Draw falling ingredient
            drawIngredient(iType, (int)ix, (int)iy);

            display.setCursor(0,0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(20, 20); display.print("RUINED BURGER!");
            display.setCursor(20, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(BurgerChefGame)
