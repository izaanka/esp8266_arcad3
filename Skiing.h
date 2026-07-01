#pragma once

class SkiingGame : public ArcadeGame {
private:
    float pX = 64;
    int score = 0;
    int state = 0;
    float speed = 2.0;
    
    struct Gate { float x, y, width; bool passed; };
    Gate gates[3];
    
    struct Tree { float x, y; };
    Tree trees[4];

    void spawnGate(int i, float y) {
        gates[i].y = y;
        gates[i].width = random(25, 40);
        gates[i].x = random(10, 128 - gates[i].width - 10);
        gates[i].passed = false;
    }
    
    void spawnTree(int i, float y) {
        trees[i].y = y;
        trees[i].x = random(0, 128);
    }

public:
    const char* getCategory() override { return "Sports"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Slalom Skiing"; }

    void init() override {
        pX = 64; score = 0; state = 0; speed = 2.0;
        spawnGate(0, 64); spawnGate(1, 114); spawnGate(2, 164);
        for(int i=0; i<4; i++) spawnTree(i, random(64, 128));
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) pX -= 3.0;
            if (digitalRead(btnRight) == LOW) pX += 3.0;
            if (pX < 2) pX = 2; if (pX > 125) pX = 125;
            
            float pY = 15; // Skier fixed Y
            
            // Move Gates
            for(int i=0; i<3; i++) {
                gates[i].y -= speed;
                
                // Passed through
                if (gates[i].y < pY && !gates[i].passed) {
                    gates[i].passed = true;
                    if (pX > gates[i].x && pX < gates[i].x + gates[i].width) {
                        score++;
                        if (score % 10 == 0) speed += 0.2;
                    } else {
                        state = 1; // Missed gate!
                        delay(500);
                    }
                }
                
                // Recycle
                if (gates[i].y < -10) {
                    spawnGate(i, 140);
                }
            }
            
            // Move Trees
            for(int i=0; i<4; i++) {
                trees[i].y -= speed;
                
                if (abs(trees[i].x - pX) < 4 && abs(trees[i].y - pY) < 4) {
                    state = 1; // Hit tree
                    delay(500);
                }
                
                if (trees[i].y < -10) {
                    spawnTree(i, random(130, 150));
                }
            }
            
            display.clearDisplay();
            
            // Draw Skier
            display.fillTriangle((int)pX, (int)pY - 2, (int)pX - 3, (int)pY + 4, (int)pX + 3, (int)pY + 4, WHITE);
            display.drawLine((int)pX - 4, (int)pY, (int)pX - 2, (int)pY + 8, WHITE); // Skis
            display.drawLine((int)pX + 4, (int)pY, (int)pX + 2, (int)pY + 8, WHITE);
            
            // Draw Gates
            for(int i=0; i<3; i++) {
                if (gates[i].y > 0 && gates[i].y < 64) {
                    // Left flag
                    display.drawLine((int)gates[i].x, (int)gates[i].y, (int)gates[i].x, (int)gates[i].y - 8, WHITE);
                    display.fillTriangle((int)gates[i].x, (int)gates[i].y - 8, (int)gates[i].x, (int)gates[i].y - 4, (int)gates[i].x + 4, (int)gates[i].y - 6, WHITE);
                    // Right flag
                    float rx = gates[i].x + gates[i].width;
                    display.drawLine((int)rx, (int)gates[i].y, (int)rx, (int)gates[i].y - 8, WHITE);
                    display.fillTriangle((int)rx, (int)gates[i].y - 8, (int)rx, (int)gates[i].y - 4, (int)rx - 4, (int)gates[i].y - 6, WHITE);
                }
            }
            
            // Draw Trees
            for(int i=0; i<4; i++) {
                if (trees[i].y > 0 && trees[i].y < 64) {
                    display.drawLine((int)trees[i].x, (int)trees[i].y, (int)trees[i].x, (int)trees[i].y - 4, WHITE); // trunk
                    display.fillTriangle((int)trees[i].x, (int)trees[i].y - 12, (int)trees[i].x - 4, (int)trees[i].y - 4, (int)trees[i].x + 4, (int)trees[i].y - 4, WHITE); // leaves
                }
            }
            
            display.setCursor(0,0); display.print(score);
            
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("WIPEOUT!");
            display.setCursor(35, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(SkiingGame)
