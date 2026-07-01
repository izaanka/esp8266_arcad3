#pragma once

class BlockJumpGame : public ArcadeGame {
private:
    float gdPlayerY = 54, gdPlayerVY = 0;
    const float gdGravity = 0.6;
    const float gdJumpForce = -5.5;
    bool gdIsGrounded = true;
    int gdObsX = 128, gdObsW = 8, gdObsH = 12;
    int gdScore = 0;
    int state = 30;

public:
    const char* getCategory() override { return "Puzzle"; }
    virtual int getScore() override { return gdScore; }
    const char* getName() override { return "Block Jump"; }

    void init() override {
        gdPlayerY = 54;
        gdPlayerVY = 0;
        gdIsGrounded = true;
        gdObsX = 128;
        gdObsW = 8;
        gdObsH = random(8, 16); 
        gdScore = 0;
        state = 30;
    }

    void update() override {
        if (state == 30) { 
            if (digitalRead(btnSelect) == LOW && gdIsGrounded) {
              gdPlayerVY = gdJumpForce;
              gdIsGrounded = false;
            }
        
            gdPlayerVY += gdGravity;
            gdPlayerY += gdPlayerVY;
            if (gdPlayerY >= 54) {
              gdPlayerY = 54;
              gdPlayerVY = 0;
              gdIsGrounded = true;
            }
        
            gdObsX -= 4; 
            
            if (gdObsX < -gdObsW) {
              gdObsX = 128;
              gdObsW = random(6, 12); 
              gdObsH = random(8, 20); 
              gdScore++;
            }
        
            int playerLeft = 20;
            int playerRight = 28;
            int playerTop = gdPlayerY;
            int playerBottom = gdPlayerY + 8;
            
            int obsLeft = gdObsX;
            int obsRight = gdObsX + gdObsW;
            int obsTop = 62 - gdObsH;
            int obsBottom = 62;
            
            if (playerRight > obsLeft && playerLeft < obsRight && playerBottom > obsTop && playerTop < obsBottom) {
              state = 31;
              delay(500); 
            }
        
            display.clearDisplay();
            display.drawLine(0, 62, 128, 62, WHITE); 
            display.fillRect(playerLeft, gdPlayerY, 8, 8, WHITE);
            display.fillRect(gdObsX, obsTop, gdObsW, gdObsH, WHITE); 
            
            display.setCursor(0, 0); 
            display.print("Score: "); display.print(gdScore);
            
            display.display();
            delay(15);
        }
        else if (state == 31) { 
            display.clearDisplay();
            display.setCursor(20, 10); display.print("GAME OVER");
            display.setCursor(20, 25);
            display.print("Score: "); display.print(gdScore);
            display.setCursor(20, 45); display.print("D6: Main Menu");
            
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(BlockJumpGame)
