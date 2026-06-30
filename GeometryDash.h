#pragma once

class GeometryDashGame : public ArcadeGame {
private:
    float pY = 54, pVY = 0;
    const float gravity = 0.7;
    const float jumpForce = -6.0;
    bool isGrounded = true;
    
    float spikeX = 128;
    int spikeCount = 1; // 1 to 3 consecutive spikes
    float speed = 3.0; // horizontal speed
    
    int score = 0;
    int state = 500;
    bool lastSelect = true;

public:
    const char* getName() override { return "Geometry Dash"; }

    void init() override {
        pY = 54;
        pVY = 0;
        isGrounded = true;
        spikeX = 128;
        spikeCount = 1;
        speed = 3.0;
        score = 0;
        state = 500;
        lastSelect = true;
    }

    void update() override {
        if (state == 500) {
            bool currSelect = digitalRead(btnSelect);
            if (currSelect == LOW && lastSelect == HIGH && isGrounded) {
                pVY = jumpForce;
                isGrounded = false;
            }
            lastSelect = currSelect;

            pVY += gravity;
            pY += pVY;
            
            if (pY >= 54) {
                pY = 54;
                pVY = 0;
                isGrounded = true;
            }

            spikeX -= speed;
            
            if (spikeX < -(spikeCount * 12)) {
                spikeX = 128 + random(0, 40); // Random distance before next spike group
                spikeCount = random(1, 4); // 1, 2, or 3 spikes
                score++;
                if (score % 5 == 0 && speed < 7.0) speed += 0.5; // Speed up capping at 7
            }

            // Collision (generous hitbox for triangles)
            int pLeft = 20;
            int pRight = 28;
            int pBottom = pY + 8;
            
            int sLeft = (int)spikeX + 2;
            int sRight = (int)spikeX + (spikeCount * 12) - 2;
            int sTop = 56; // Give a couple pixels of leniency on the tip

            if (pRight > sLeft && pLeft < sRight && pBottom > sTop) {
                state = 501;
                delay(500);
            }

            display.clearDisplay();
            
            // Draw Ground
            display.drawLine(0, 62, 128, 62, WHITE);
            
            // Draw Player Cube
            display.drawRect(pLeft, pY, 8, 8, WHITE);
            display.fillRect(pLeft + 2, pY + 2, 4, 4, WHITE); // Inner detail
            
            // Draw Spikes
            for (int i = 0; i < spikeCount; i++) {
                int sx = (int)spikeX + (i * 12);
                display.fillTriangle(sx, 62, sx + 6, 54, sx + 12, 62, WHITE);
            }

            display.setCursor(0, 0);
            display.print(score);
            
            display.display();
            delay(15);
        } else if (state == 501) {
            display.clearDisplay();
            display.setCursor(20, 20); display.print("CRASHED!");
            display.setCursor(20, 35); display.print("Score: "); display.print(score);
            display.setCursor(20, 50); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(GeometryDashGame)
