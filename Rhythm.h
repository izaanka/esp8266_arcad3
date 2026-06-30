#pragma once

class RhythmGame : public ArcadeGame {
private:
    struct Note { float y; int col; bool active; };
    Note notes[10];
    
    int score = 0;
    int combo = 0;
    int maxCombo = 0;
    float speed = 2.0;
    int state = 0;
    
    bool btnL = true, btnS = true, btnR = true;

    void spawnNote() {
        for(int i=0; i<10; i++) {
            if (!notes[i].active) {
                notes[i].active = true;
                notes[i].y = -10;
                notes[i].col = random(0, 3);
                break;
            }
        }
    }

public:
    const char* getName() override { return "Rhythm Hero"; }

    void init() override {
        score = 0; combo = 0; maxCombo = 0; speed = 2.0; state = 0;
        for(int i=0; i<10; i++) notes[i].active = false;
        btnL = true; btnS = true; btnR = true;
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);
            
            bool hit[3] = { (cL && !btnL), (cS && !btnS), (cR && !btnR) };
            
            btnL = cL; btnS = cS; btnR = cR;
            
            // Move notes
            if (random(100) < 5 + (speed*1.5)) spawnNote();
            
            for(int i=0; i<10; i++) {
                if (notes[i].active) {
                    notes[i].y += speed;
                    
                    if (notes[i].y > 64) {
                        notes[i].active = false;
                        combo = 0; // Missed note!
                    }
                }
            }
            
            // Handle hits
            for (int col=0; col<3; col++) {
                if (hit[col]) {
                    // Find lowest note in this col
                    int bestNote = -1;
                    float bestY = -100;
                    for(int i=0; i<10; i++) {
                        if (notes[i].active && notes[i].col == col) {
                            if (notes[i].y > bestY) {
                                bestY = notes[i].y;
                                bestNote = i;
                            }
                        }
                    }
                    
                    if (bestNote != -1) {
                        // Check distance to target line (y=50)
                        float dist = abs(notes[bestNote].y - 50);
                        if (dist < 8) { // Hit!
                            notes[bestNote].active = false;
                            combo++;
                            if (combo > maxCombo) maxCombo = combo;
                            score += 10 + (combo * 2);
                            if (score % 200 == 0) speed += 0.2; // Speed up
                        } else if (dist < 15) {
                            // Too early/late, miss!
                            combo = 0;
                        }
                    }
                }
            }
            
            display.clearDisplay();
            
            // Draw Tracks
            int cx[3] = {32, 64, 96};
            for(int i=0; i<3; i++) {
                display.drawLine(cx[i]-6, 0, cx[i]-6, 64, WHITE);
                display.drawLine(cx[i]+6, 0, cx[i]+6, 64, WHITE);
                
                // Target area
                display.drawRect(cx[i]-5, 48, 10, 4, WHITE);
            }
            
            // Draw Notes
            for(int i=0; i<10; i++) {
                if (notes[i].active) {
                    display.fillRect(cx[notes[i].col]-4, (int)notes[i].y, 8, 4, WHITE);
                }
            }
            
            // Draw UI
            display.setCursor(0,0); display.print(score);
            display.setCursor(110,0); display.print(combo);
            
            // Exit condition (let's say they can play until they want to quit by holding left+right?)
            // Or just a simple endless game. 
            // We'll let them play endlessly. To exit, standard d5 timer applies from main loop.
            
            display.display();
            delay(15);
        }
    }
};

REGISTER_GAME(RhythmGame)
