#pragma once

class SimonSaysGame : public ArcadeGame {
private:
    int simonSeq[32];
    int simonStep = 0;
    int simonRound = 0;
    int simonCursor = 0;
    int state = 40;

    void drawSimonGrid(int highlight) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(35, 2);
      display.print("SIMON SAYS");
      for (int i = 0; i < 4; i++) {
        int drawX = 12 + i * 26;
        int drawY = 24;
        display.drawRect(drawX, drawY, 22, 22, WHITE);
        if (i == highlight) {
          display.fillRect(drawX + 3, drawY + 3, 16, 16, WHITE);
        }
        if (state == 41 && simonCursor == i) {
          display.drawRect(drawX - 2, drawY - 2, 26, 26, WHITE);
        }
      }
      display.setCursor(10, 52);
      display.print("Round: "); display.print(simonRound);
      display.display();
    }

public:
    const char* getName() override { return "Simon Says"; }

    void init() override {
        simonRound = 1;
        simonStep = 0;
        simonCursor = 0;
        for (int i = 0; i < 32; i++) {
          simonSeq[i] = random(4);
        }
        state = 40; 
    }

    void update() override {
        if (state == 40) {
            delay(800);
            for (int i = 0; i < simonRound; i++) {
              drawSimonGrid(simonSeq[i]);
              delay(400);
              drawSimonGrid(-1);
              delay(150);
            }
            state = 41; 
            simonStep = 0;
        }
        else if (state == 41) {
            if (digitalRead(btnLeft) == LOW) { simonCursor = (simonCursor > 0) ? simonCursor - 1 : 3; delay(150); }
            if (digitalRead(btnRight) == LOW) { simonCursor = (simonCursor < 3) ? simonCursor + 1 : 0; delay(150); }
            if (digitalRead(btnSelect) == LOW) {
              delay(200);
              if (simonCursor == simonSeq[simonStep]) {
                simonStep++;
                if (simonStep >= simonRound) {
                  display.clearDisplay();
                  display.setCursor(35, 25);
                  display.print("CORRECT!");
                  display.display();
                  delay(1000);
                  
                  simonRound++;
                  if (simonRound >= 32) {
                    simonRound = 99; // Victory Flag
                    state = 42;
                  } else {
                    simonSeq[simonRound - 1] = random(4);
                    state = 40; 
                  }
                }
              } else {
                state = 42; // Failure
              }
            }
            if (state == 41) drawSimonGrid(-1);
        }
        else if (state == 42) {
            display.clearDisplay();
            display.setCursor(20, 15);
            if (simonRound == 99) {
              display.print("VICTORY WON!");
            } else {
              display.print("GAME OVER!");
              display.setCursor(20, 30);
              display.print("Reached Round: "); display.print(simonRound);
            }
            display.setCursor(20, 50);
            display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(SimonSaysGame)
