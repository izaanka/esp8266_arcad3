#pragma once

class TrafficCopGame : public ArcadeGame {
private:
    struct Car { float x, y, vx, vy; bool active; };
    Car cars[4]; // Max 4 cars at once
    int lightH, lightV; // 0=Red, 1=Green
    int score;
    int state;
    bool lastS;

    void spawnCar() {
        for(int i=0; i<4; i++) {
            if (!cars[i].active) {
                cars[i].active = true;
                int dir = random(4); // 0=Up, 1=Down, 2=Left, 3=Right
                if (dir == 0) { cars[i].x = 58; cars[i].y = 64; cars[i].vx = 0; cars[i].vy = -1; }
                else if (dir == 1) { cars[i].x = 70; cars[i].y = 0; cars[i].vx = 0; cars[i].vy = 1; }
                else if (dir == 2) { cars[i].x = 128; cars[i].y = 26; cars[i].vx = -1; cars[i].vy = 0; }
                else if (dir == 3) { cars[i].x = 0; cars[i].y = 38; cars[i].vx = 1; cars[i].vy = 0; }
                break;
            }
        }
    }

public:
    const char* getCategory() override { return "Action"; }
    const char* getName() override { return "Traffic Cop"; }
    int getScore() override { return score; }

    void init() override {
        score = 0; state = 0; lightH = 0; lightV = 1; lastS = true;
        for(int i=0; i<4; i++) cars[i].active = false;
    }

    void update() override {
        if (state == 0) {
            bool cS = (digitalRead(btnSelect) == LOW);
            if (cS && !lastS) { // Toggle lights
                lightH = 1 - lightH;
                lightV = 1 - lightV;
            }
            lastS = cS;

            if (random(100) < 5 + (score*0.1)) spawnCar();

            // Move cars
            for(int i=0; i<4; i++) {
                if (cars[i].active) {
                    // Check stopping at light
                    bool canMove = true;
                    if (cars[i].vx > 0 && cars[i].x > 45 && cars[i].x < 55 && lightH == 0) canMove = false; // from left
                    if (cars[i].vx < 0 && cars[i].x < 83 && cars[i].x > 73 && lightH == 0) canMove = false; // from right
                    if (cars[i].vy > 0 && cars[i].y > 13 && cars[i].y < 23 && lightV == 0) canMove = false; // from top
                    if (cars[i].vy < 0 && cars[i].y < 51 && cars[i].y > 41 && lightV == 0) canMove = false; // from bottom

                    // Check bumping into other stopped cars (simplified)
                    for(int j=0; j<4; j++) {
                        if (i!=j && cars[j].active) {
                            if (cars[i].vx != 0 && cars[i].vx == cars[j].vx && abs(cars[i].y - cars[j].y) < 4) {
                                if (cars[i].vx > 0 && cars[i].x < cars[j].x && cars[j].x - cars[i].x < 15) canMove = false;
                                if (cars[i].vx < 0 && cars[i].x > cars[j].x && cars[i].x - cars[j].x < 15) canMove = false;
                            }
                            if (cars[i].vy != 0 && cars[i].vy == cars[j].vy && abs(cars[i].x - cars[j].x) < 4) {
                                if (cars[i].vy > 0 && cars[i].y < cars[j].y && cars[j].y - cars[i].y < 15) canMove = false;
                                if (cars[i].vy < 0 && cars[i].y > cars[j].y && cars[i].y - cars[j].y < 15) canMove = false;
                            }
                        }
                    }

                    if (canMove) {
                        cars[i].x += cars[i].vx; cars[i].y += cars[i].vy;
                    }

                    // Score & Despawn
                    if (cars[i].x < -10 || cars[i].x > 138 || cars[i].y < -10 || cars[i].y > 74) {
                        cars[i].active = false; score++;
                    }

                    // Crash check (intersection overlap)
                    for(int j=0; j<4; j++) {
                        if (i!=j && cars[j].active) {
                            if (abs(cars[i].x - cars[j].x) < 10 && abs(cars[i].y - cars[j].y) < 10) {
                                state = 1; delay(500); // Crash!
                            }
                        }
                    }
                }
            }

            display.clearDisplay();
            
            // Draw Roads
            display.drawLine(52, 0, 52, 20, WHITE);
            display.drawLine(52, 44, 52, 64, WHITE);
            display.drawLine(76, 0, 76, 20, WHITE);
            display.drawLine(76, 44, 76, 64, WHITE);
            display.drawLine(0, 20, 52, 20, WHITE);
            display.drawLine(76, 20, 128, 20, WHITE);
            display.drawLine(0, 44, 52, 44, WHITE);
            display.drawLine(76, 44, 128, 44, WHITE);
            
            // Draw Lights
            if (lightH == 0) {
                display.fillCircle(48, 16, 2, WHITE); display.fillCircle(80, 48, 2, WHITE);
            } else {
                display.drawCircle(48, 16, 2, WHITE); display.drawCircle(80, 48, 2, WHITE);
            }
            if (lightV == 0) {
                display.fillCircle(72, 16, 2, WHITE); display.fillCircle(56, 48, 2, WHITE);
            } else {
                display.drawCircle(72, 16, 2, WHITE); display.drawCircle(56, 48, 2, WHITE);
            }

            // Draw Cars
            for(int i=0; i<4; i++) {
                if (cars[i].active) {
                    if (cars[i].vx != 0) display.fillRect((int)cars[i].x - 6, (int)cars[i].y - 4, 12, 8, WHITE);
                    else display.fillRect((int)cars[i].x - 4, (int)cars[i].y - 6, 8, 12, WHITE);
                }
            }

            display.setCursor(0,0); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(40, 20); display.print("CRASH!");
            display.setCursor(40, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(TrafficCopGame)
