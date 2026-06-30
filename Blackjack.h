#pragma once

class BlackjackGame : public ArcadeGame {
private:
    int pCards[10];
    int pCount = 0;
    int dCards[10];
    int dCount = 0;
    
    int state = 0; // 0=player turn, 1=dealer turn, 2=over
    bool lastL = true, lastR = true, lastS = true;

    int drawCard() {
        int v = random(1, 14);
        return v; // 1=A, 11=J, 12=Q, 13=K
    }
    
    int calcTotal(int* cards, int count) {
        int total = 0;
        int aces = 0;
        for(int i=0; i<count; i++) {
            if (cards[i] == 1) { aces++; total += 11; }
            else if (cards[i] >= 10) { total += 10; }
            else { total += cards[i]; }
        }
        while (total > 21 && aces > 0) {
            total -= 10;
            aces--;
        }
        return total;
    }
    
    void printCardName(int v) {
        if (v == 1) display.print("A");
        else if (v == 11) display.print("J");
        else if (v == 12) display.print("Q");
        else if (v == 13) display.print("K");
        else display.print(v);
    }

public:
    const char* getName() override { return "Blackjack 21"; }

    void init() override {
        pCount = 0; dCount = 0;
        pCards[pCount++] = drawCard();
        dCards[dCount++] = drawCard();
        pCards[pCount++] = drawCard();
        dCards[dCount++] = drawCard();
        state = 0;
        lastL = true; lastR = true; lastS = true;
    }

    void update() override {
        int pTotal = calcTotal(pCards, pCount);
        int dTotal = calcTotal(dCards, dCount);
        
        bool cL = (digitalRead(btnLeft) == LOW);
        bool cS = (digitalRead(btnSelect) == LOW);
        bool cR = (digitalRead(btnRight) == LOW);
        
        if (state == 0) {
            // Player turn
            if (pTotal >= 21) {
                state = 1;
                delay(500);
            } else {
                if (cL && !lastL) { // Hit
                    pCards[pCount++] = drawCard();
                }
                if (cR && !lastR) { // Stand
                    state = 1;
                    delay(500);
                }
            }
        } else if (state == 1) {
            // Dealer turn
            if (pTotal > 21) {
                state = 2; // Dealer doesn't need to draw if player busts
            } else if (dTotal < 17) {
                dCards[dCount++] = drawCard();
                delay(500);
            } else {
                state = 2;
            }
        } else if (state == 2) {
            if (cS && !lastS) { init(); delay(200); }
            if (cL && !lastL) { returnToMenu(); delay(200); }
        }
        
        lastL = cL; lastR = cR; lastS = cS;
        
        display.clearDisplay();
        
        // Draw Dealer
        display.setCursor(0, 0); display.print("DEALER: ");
        if (state == 0) display.print("?"); // hide total
        else display.print(dTotal);
        
        display.setCursor(0, 10);
        for(int i=0; i<dCount; i++) {
            if (state == 0 && i == 1) display.print("[?] "); // Hole card
            else { display.print("["); printCardName(dCards[i]); display.print("] "); }
        }
        
        // Draw Player
        display.setCursor(0, 30); display.print("PLAYER: "); display.print(pTotal);
        display.setCursor(0, 40);
        for(int i=0; i<pCount; i++) {
            display.print("["); printCardName(pCards[i]); display.print("] ");
        }
        
        // Controls / Result
        display.setCursor(0, 56);
        if (state == 0) {
            display.print("D5: HIT     D7: STAND");
        } else if (state == 2) {
            if (pTotal > 21) display.print("BUST! YOU LOSE.");
            else if (dTotal > 21) display.print("DEALER BUSTS! WIN!");
            else if (pTotal > dTotal) display.print("YOU WIN!");
            else if (pTotal < dTotal) display.print("YOU LOSE.");
            else display.print("PUSH (TIE).");
        }
        
        display.display();
        delay(15);
    }
};

REGISTER_GAME(BlackjackGame)
