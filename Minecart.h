#pragma once

class MinecartGame : public ArcadeGame {
private:
    int lane; // 0, 1, 2
    int score;
    int state;
    bool lastL, lastR;
    
    struct Block { float y; int lane; };
    Block blocks[3];
    float speed;

public:
    const char* getCategory() override { return "Action"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Minecart"; }

    void init() override {
        lane = 1; score = 0; state = 0; speed = 2.0;
        lastL = true; lastR = true;
        for(int i=0; i<3; i++) {
            blocks[i].y = -i * 40;
            blocks[i].lane = random(3);
        }
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);
            
            if (cL && !lastL && lane > 0) lane--;
            if (cR && !lastR && lane < 2) lane++;
            
            lastL = cL; lastR = cR;

            for(int i=0; i<3; i++) {
                blocks[i].y += speed;
                if (blocks[i].y > 64) {
                    blocks[i].y = -20;
                    blocks[i].lane = random(3);
                    score++;
                    if (score % 10 == 0) speed += 0.5;
                }

                // Collision
                if (blocks[i].lane == lane && blocks[i].y > 44 && blocks[i].y < 56) {
                    state = 1; delay(500);
                }
            }

            display.clearDisplay();
            
            // Tracks
            display.drawLine(42, 0, 42, 64, WHITE);
            display.drawLine(85, 0, 85, 64, WHITE);

            // Blocks
            for(int i=0; i<3; i++) {
                int bx = 10 + blocks[i].lane * 43;
                display.fillRect(bx, (int)blocks[i].y, 22, 6, WHITE);
                display.drawLine(bx, (int)blocks[i].y+2, bx+22, (int)blocks[i].y+4, BLACK);
            }

            // Player cart
            int px = 10 + lane * 43;
            display.fillRect(px+2, 50, 18, 12, WHITE);
            display.fillCircle(px+4, 62, 2, WHITE);
            display.fillCircle(px+16, 62, 2, WHITE);

            display.setCursor(0, 0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("CRASHED!");
            display.setCursor(35, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(MinecartGame)
