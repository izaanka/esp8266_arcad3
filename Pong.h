#pragma once

class PongGame : public ArcadeGame {
private:
    float p1Y = 24, p2Y = 24; // p1=player, p2=AI
    float bX = 64, bY = 32;
    float bVX = 2.0, bVY = 1.0;
    int p1Score = 0, p2Score = 0;
    int state = 0;

    void resetBall() {
        bX = 64; bY = 32;
        bVX = (random(2)==0?2.0:-2.0);
        bVY = random(-15, 15) / 10.0;
        if (bVY == 0) bVY = 1.0;
    }

public:
    const char* getCategory() override { return "Sports"; }
    virtual int getScore() override { return p1Score; }
    const char* getName() override { return "Pong vs AI"; }

    void init() override {
        p1Y = 24; p2Y = 24;
        p1Score = 0; p2Score = 0;
        state = 0;
        resetBall();
    }

    void update() override {
        if (state == 0) {
            // Player Control (Left paddle)
            if (digitalRead(btnLeft) == LOW) p1Y -= 2.0;
            if (digitalRead(btnRight) == LOW) p1Y += 2.0;
            
            // Clamp
            if (p1Y < 0) p1Y = 0; if (p1Y > 48) p1Y = 48; // paddle is 16px high
            
            // AI Control (Right paddle)
            if (bVX > 0) { // Ball moving towards AI
                if (bY > p2Y + 8) p2Y += 1.5;
                else if (bY < p2Y + 8) p2Y -= 1.5;
            }
            if (p2Y < 0) p2Y = 0; if (p2Y > 48) p2Y = 48;
            
            // Ball Physics
            bX += bVX; bY += bVY;
            
            if (bY <= 0 || bY >= 62) { bVY = -bVY; } // Top/Bottom bounce
            
            // Paddle Collisions
            if (bX <= 8 && bX >= 4 && bY >= p1Y && bY <= p1Y + 16) {
                bX = 9; bVX = -bVX * 1.1; // Speed up
                bVY = (bY - (p1Y + 8)) * 0.2; // Angle change based on hit point
            }
            if (bX >= 118 && bX <= 122 && bY >= p2Y && bY <= p2Y + 16) {
                bX = 117; bVX = -bVX * 1.1;
                bVY = (bY - (p2Y + 8)) * 0.2;
            }
            
            // Speed limit
            if (bVX > 5.0) bVX = 5.0; if (bVX < -5.0) bVX = -5.0;
            
            // Score
            if (bX < 0) { p2Score++; resetBall(); delay(500); }
            if (bX > 128) { p1Score++; resetBall(); delay(500); }
            
            if (p1Score >= 5 || p2Score >= 5) { state = 1; delay(500); }
            
            // Draw
            display.clearDisplay();
            
            // Paddles
            display.fillRect(4, (int)p1Y, 4, 16, WHITE);
            display.fillRect(120, (int)p2Y, 4, 16, WHITE);
            
            // Ball
            display.fillRect((int)bX, (int)bY, 3, 3, WHITE);
            
            // Center Line & Score
            for (int i=0; i<64; i+=8) display.drawLine(64, i, 64, i+4, WHITE);
            display.setCursor(40, 5); display.print(p1Score);
            display.setCursor(80, 5); display.print(p2Score);
            
            display.display();
            delay(10);
            
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); 
            if (p1Score > p2Score) display.print("YOU WIN!"); else display.print("YOU LOSE!");
            display.setCursor(20, 50); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(PongGame)
