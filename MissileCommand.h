#pragma once

class MissileCommandGame : public ArcadeGame {
private:
    float angle = PI / 2; // pointing straight up
    int score = 0;
    int state = 0;
    
    struct Meteor { float x, y, vx, vy; bool active; };
    Meteor meteors[5];
    
    struct Explosion { float x, y; int timer; bool active; };
    Explosion explosions[3];
    
    bool lastSelect = true;

    void spawnMeteor() {
        for(int i=0; i<5; i++) {
            if (!meteors[i].active) {
                meteors[i].active = true;
                meteors[i].x = random(0, 128);
                meteors[i].y = -10;
                
                float targetX = random(10, 118);
                float targetY = 64;
                float dx = targetX - meteors[i].x;
                float dy = targetY - meteors[i].y;
                float dist = sqrt(dx*dx + dy*dy);
                float speed = random(5, 15) / 10.0 + score*0.05;
                
                meteors[i].vx = (dx/dist) * speed;
                meteors[i].vy = (dy/dist) * speed;
                break;
            }
        }
    }

public:
    const char* getName() override { return "Missile Command"; }

    void init() override {
        angle = PI / 2; score = 0; state = 0; lastSelect = true;
        for(int i=0; i<5; i++) meteors[i].active = false;
        for(int i=0; i<3; i++) explosions[i].active = false;
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) angle += 0.05; // rotate left (counter-clockwise)
            if (digitalRead(btnRight) == LOW) angle -= 0.05;
            
            // Limit angle from ~30 deg to 150 deg
            if (angle < 0.2) angle = 0.2;
            if (angle > PI - 0.2) angle = PI - 0.2;
            
            bool currSelect = (digitalRead(btnSelect) == LOW);
            if (currSelect && !lastSelect) {
                // Fire beam! It instantly creates an explosion at a fixed distance, or we just draw a line and check intersection.
                // Let's do a beam that extends out and creates an explosion at Y=32 (mid sky)
                // Actually, let's just make the beam instantly destroy anything it touches.
                
                // Beam is a line from (64, 62) to (64 + cos*100, 62 - sin*100)
                float bx1 = 64, by1 = 62;
                float bx2 = 64 + cos(angle)*100;
                float by2 = 62 - sin(angle)*100;
                
                // Draw flash
                display.drawLine(bx1, by1, bx2, by2, WHITE);
                
                // Check collisions with meteors
                for(int i=0; i<5; i++) {
                    if (meteors[i].active) {
                        // Point to line distance
                        float px = meteors[i].x; float py = meteors[i].y;
                        
                        // Line vector
                        float lvx = bx2 - bx1; float lvy = by2 - by1;
                        float len2 = lvx*lvx + lvy*lvy;
                        
                        // Dot product to find projection
                        float t = ((px - bx1)*lvx + (py - by1)*lvy) / len2;
                        
                        if (t >= 0 && t <= 1) { // within beam segment
                            float projX = bx1 + t*lvx;
                            float projY = by1 + t*lvy;
                            float dist2 = (px-projX)*(px-projX) + (py-projY)*(py-projY);
                            
                            if (dist2 < 36) { // hit radius 6
                                meteors[i].active = false;
                                score += 10;
                                
                                // Spawn explosion visual
                                for(int e=0; e<3; e++) {
                                    if (!explosions[e].active) {
                                        explosions[e].active = true;
                                        explosions[e].x = px; explosions[e].y = py;
                                        explosions[e].timer = 15;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            lastSelect = currSelect;
            
            if (random(100) < 3) spawnMeteor();
            
            for(int i=0; i<5; i++) {
                if (meteors[i].active) {
                    meteors[i].x += meteors[i].vx;
                    meteors[i].y += meteors[i].vy;
                    if (meteors[i].y > 64) {
                        state = 1; // City destroyed
                        delay(500);
                    }
                }
            }
            
            for(int i=0; i<3; i++) {
                if (explosions[i].active) {
                    explosions[i].timer--;
                    if (explosions[i].timer <= 0) explosions[i].active = false;
                }
            }
            
            display.clearDisplay();
            
            // Draw City line
            display.drawLine(0, 62, 128, 62, WHITE);
            display.fillRect(10, 58, 20, 4, WHITE);
            display.fillRect(98, 56, 16, 6, WHITE);
            
            // Draw Cannon
            display.fillCircle(64, 62, 6, WHITE);
            display.drawLine(64, 62, 64 + cos(angle)*12, 62 - sin(angle)*12, WHITE);
            
            // Draw Meteors
            for(int i=0; i<5; i++) {
                if (meteors[i].active) {
                    display.fillCircle((int)meteors[i].x, (int)meteors[i].y, 2, WHITE);
                    display.drawLine((int)meteors[i].x, (int)meteors[i].y, (int)(meteors[i].x - meteors[i].vx*3), (int)(meteors[i].y - meteors[i].vy*3), WHITE); // trail
                }
            }
            
            // Draw Explosions
            for(int i=0; i<3; i++) {
                if (explosions[i].active) {
                    int r = 15 - explosions[i].timer;
                    display.drawCircle((int)explosions[i].x, (int)explosions[i].y, r, WHITE);
                }
            }
            
            display.setCursor(0, 0); display.print(score);
            
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(20, 20); display.print("CITY DESTROYED!");
            display.setCursor(20, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(MissileCommandGame)
