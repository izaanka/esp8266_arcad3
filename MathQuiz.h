#pragma once

class MathQuizGame : public ArcadeGame {
private:
    int n1, n2, op;
    int correctAns;
    int options[3];
    int correctSlot;
    int score = 0;
    int state = 0; 
    bool btnL = true, btnS = true, btnR = true;

    void generateQuestion() {
        n1 = random(10, 100);
        n2 = random(10, 100);
        op = random(0, 3);
        
        if (op == 1 && n1 < n2) {
            int temp = n1; n1 = n2; n2 = temp; // Ensure positive subtraction
        }
        
        if (op == 0) correctAns = n1 + n2;
        else if (op == 1) correctAns = n1 - n2;
        else correctAns = n1 * n2;
        
        correctSlot = random(0, 3);
        options[correctSlot] = correctAns;
        
        for(int i=0; i<3; i++) {
            if (i != correctSlot) {
                int fake;
                do {
                    fake = correctAns + random(-20, 20);
                    if (op == 2) fake = correctAns + random(-100, 100);
                } while (fake == correctAns || fake == options[0] || fake == options[1] || fake == options[2]);
                options[i] = fake;
            }
        }
    }

public:
    const char* getCategory() override { return "Puzzle"; }
    virtual int getScore() override { return score; }
    const char* getName() override { return "Math Quiz"; }

    void init() override {
        score = 0; state = 0;
        btnL = true; btnS = true; btnR = true;
        generateQuestion();
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);
            
            int guess = -1;
            if (cL && !btnL) guess = 0;
            if (cS && !btnS) guess = 1;
            if (cR && !btnR) guess = 2;
            
            btnL = cL; btnS = cS; btnR = cR;
            
            if (guess != -1) {
                if (guess == correctSlot) {
                    score++;
                    generateQuestion();
                } else {
                    state = 1; // Wrong! Game Over.
                    delay(500);
                }
            }
            
            display.clearDisplay();
            
            display.setTextSize(2);
            display.setCursor(10, 10);
            display.print(n1);
            if (op == 0) display.print(" + ");
            else if (op == 1) display.print(" - ");
            else display.print(" x ");
            display.print(n2);
            display.setTextSize(1);
            
            display.setCursor(0,0); display.print("Score: "); display.print(score);
            
            // Draw options
            display.drawRect(0, 40, 40, 20, WHITE);
            display.drawRect(44, 40, 40, 20, WHITE);
            display.drawRect(88, 40, 40, 20, WHITE);
            
            display.setCursor(5, 46); display.print(options[0]);
            display.setCursor(49, 46); display.print(options[1]);
            display.setCursor(93, 46); display.print(options[2]);
            
            display.display();
            delay(15);
            
        } else {
            display.clearDisplay();
            display.setCursor(35, 20); display.print("WRONG!");
            display.setCursor(35, 35); display.print("Score: "); display.print(score);
            display.setCursor(20, 50); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(MathQuizGame)
