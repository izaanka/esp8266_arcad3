#pragma once

class WordScrambleGame : public ArcadeGame {
private:
    char word[5];
    char jumbled[5];
    int cursor;
    int score;
    int state;
    bool lastL, lastR, lastS;

    const char* words[10] = {
        "PLAY", "JUMP", "CODE", "GAME", "WIFI",
        "BYTE", "BOMB", "STAR", "TIME", "HERO"
    };

    void loadLevel() {
        strcpy(word, words[random(10)]);
        strcpy(jumbled, word);
        // Scramble
        for(int i=0; i<10; i++) {
            int a = random(4); int b = random(4);
            char t = jumbled[a]; jumbled[a] = jumbled[b]; jumbled[b] = t;
        }
        // Ensure it's not solved initially
        if (strcmp(word, jumbled) == 0) {
            char t = jumbled[0]; jumbled[0] = jumbled[1]; jumbled[1] = t;
        }
        cursor = 0; state = 0;
    }

public:
    const char* getCategory() override { return "Puzzle"; }
    const char* getName() override { return "Word Scramble"; }
    int getScore() override { return score; }

    void init() override {
        score = 0; lastL = true; lastR = true; lastS = true;
        loadLevel();
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);

            if (cL && !lastL) { cursor--; if (cursor < 0) cursor = 3; }
            if (cR && !lastR) { cursor++; if (cursor > 3) cursor = 0; }
            if (cS && !lastS) {
                int next = (cursor + 1) % 4;
                char t = jumbled[cursor]; jumbled[cursor] = jumbled[next]; jumbled[next] = t;
                
                if (strcmp(word, jumbled) == 0) {
                    state = 1; score++; delay(1000); loadLevel();
                }
            }

            lastL = cL; lastR = cR; lastS = cS;

            display.clearDisplay();
            display.setTextSize(2);
            
            for(int i=0; i<4; i++) {
                int px = 20 + i*24;
                display.setCursor(px, 30); display.print(jumbled[i]);
                if (i == cursor) display.drawLine(px, 48, px+10, 48, WHITE);
            }
            
            display.setTextSize(1);
            display.setCursor(0,0); display.print("Score: "); display.print(score);
            display.display();
            delay(15);
        }
    }
};

REGISTER_GAME(WordScrambleGame)
