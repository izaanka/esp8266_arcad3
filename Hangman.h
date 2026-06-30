#pragma once

class HangmanGame : public ArcadeGame {
private:
    const char* words[12] = {
        "ARDUINO", "ESP8266", "GAMING", "DISPLAY", 
        "BUTTON", "SOLDER", "EEPROM", "ARCADE", 
        "HACKER", "CIRCUIT", "SYSTEM", "MEMORY"
    };
    
    char word[15];
    int wordLen;
    bool guessed[26];
    int mistakes;
    char cursor;
    int state; 
    bool lastSelect;

    void drawHangman(int m) {
        display.drawLine(10, 50, 40, 50, WHITE);
        display.drawLine(25, 50, 25, 10, WHITE);
        display.drawLine(25, 10, 45, 10, WHITE);
        display.drawLine(45, 10, 45, 15, WHITE); 
        
        if (m >= 1) display.drawCircle(45, 20, 5, WHITE); 
        if (m >= 2) display.drawLine(45, 25, 45, 38, WHITE); 
        if (m >= 3) display.drawLine(45, 28, 35, 35, WHITE); 
        if (m >= 4) display.drawLine(45, 28, 55, 35, WHITE); 
        if (m >= 5) display.drawLine(45, 38, 38, 48, WHITE); 
        if (m >= 6) display.drawLine(45, 38, 52, 48, WHITE); 
    }

public:
    const char* getName() override { return "Hangman"; }

    void init() override {
        strcpy(word, words[random(0, 12)]);
        wordLen = strlen(word);
        for(int i=0; i<26; i++) guessed[i] = false;
        
        mistakes = 0;
        cursor = 'A';
        state = 0;
        lastSelect = true;
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) {
                cursor--;
                if (cursor < 'A') cursor = 'Z';
                delay(150);
            }
            if (digitalRead(btnRight) == LOW) {
                cursor++;
                if (cursor > 'Z') cursor = 'A';
                delay(150);
            }
            
            bool currSelect = (digitalRead(btnSelect) == LOW);
            if (currSelect && !lastSelect) {
                int idx = cursor - 'A';
                if (!guessed[idx]) {
                    guessed[idx] = true;
                    
                    bool found = false;
                    for(int i=0; i<wordLen; i++) {
                        if (word[i] == cursor) found = true;
                    }
                    
                    if (!found) mistakes++;
                    
                    bool win = true;
                    for(int i=0; i<wordLen; i++) {
                        if (word[i] >= 'A' && word[i] <= 'Z') {
                            if (!guessed[word[i] - 'A']) win = false;
                        }
                    }
                    if (win) state = 1;
                    if (mistakes >= 6) state = 2;
                }
            }
            lastSelect = currSelect;
            
            display.clearDisplay();
            
            drawHangman(mistakes);
            
            // Draw Word
            display.setCursor(55, 25);
            for(int i=0; i<wordLen; i++) {
                if (word[i] >= 'A' && word[i] <= 'Z') {
                    if (guessed[word[i] - 'A']) display.print(word[i]);
                    else display.print('_');
                } else {
                    display.print(word[i]);
                }
                display.print(' ');
            }
            
            // Draw Alphabet
            display.setCursor(10, 56);
            for(int i=-2; i<=2; i++) {
                char c = cursor + i;
                if (c < 'A') c += 26;
                if (c > 'Z') c -= 26;
                
                if (i == 0) {
                    display.setTextColor(BLACK, WHITE);
                    display.print(c);
                    display.setTextColor(WHITE, BLACK);
                } else {
                    if (guessed[c - 'A']) display.print('.'); 
                    else display.print(c);
                }
                display.print(" ");
            }
            
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            drawHangman(mistakes);
            display.setCursor(55, 15);
            if (state == 1) display.print("YOU WIN!");
            else display.print("YOU LOSE!");
            display.setCursor(55, 30); display.print(word);
            display.setCursor(55, 45); display.print("D6: Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(HangmanGame)
