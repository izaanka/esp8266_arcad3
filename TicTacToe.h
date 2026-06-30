#pragma once

class TicTacToeGame : public ArcadeGame {
private:
    byte tttGrid[3][3]; // 0: Empty, 1: X, 2: O
    int tttCursor = 0;
    int tttTurn = 1;
    int tttWinner = 0; // 0: None, 1: X, 2: O, 3: Tie
    int tttMode = 0; // 0: 1 Player (vs CPU), 1: 2 Players
    int tttMenuOption = 0;
    int state = 69;

    void checkTTTWin() {
      for (int i = 0; i < 3; i++) {
        if (tttGrid[i][0] != 0 && tttGrid[i][0] == tttGrid[i][1] && tttGrid[i][1] == tttGrid[i][2]) tttWinner = tttGrid[i][0];
        if (tttGrid[0][i] != 0 && tttGrid[0][i] == tttGrid[1][i] && tttGrid[1][i] == tttGrid[2][i]) tttWinner = tttGrid[0][i];
      }
      if (tttGrid[0][0] != 0 && tttGrid[0][0] == tttGrid[1][1] && tttGrid[1][1] == tttGrid[2][2]) tttWinner = tttGrid[0][0];
      if (tttGrid[0][2] != 0 && tttGrid[0][2] == tttGrid[1][1] && tttGrid[1][1] == tttGrid[2][0]) tttWinner = tttGrid[0][2];
      
      if (tttWinner == 0) {
        bool full = true;
        for (int y = 0; y < 3; y++) {
          for (int x = 0; x < 3; x++) {
            if (tttGrid[y][x] == 0) full = false;
          }
        }
        if (full) tttWinner = 3;
      }
      
      if (tttWinner != 0) state = 71;
    }

public:
    const char* getName() override { return "Tic-Tac-Toe"; }

    void init() override {
        tttMenuOption = 0;
        state = 69; 
    }

    void update() override {
        if (state == 69) { // TTT Mode Selection
            if (digitalRead(btnLeft) == LOW) { tttMenuOption = 0; delay(150); }
            if (digitalRead(btnRight) == LOW) { tttMenuOption = 1; delay(150); }
            
            if (digitalRead(btnSelect) == LOW) {
              tttMode = tttMenuOption;
              for (int y = 0; y < 3; y++) {
                for (int x = 0; x < 3; x++) tttGrid[y][x] = 0;
              }
              tttCursor = 0;
              tttTurn = 1;
              tttWinner = 0;
              state = 70;
              delay(200);
            }
            
            display.clearDisplay();
            display.setCursor(20, 10); display.print("TIC-TAC-TOE");
            display.setCursor(10, 30);
            if (tttMenuOption == 0) display.print("> ");
            display.print("1 Player (vs CPU)");
            display.setCursor(10, 45);
            if (tttMenuOption == 1) display.print("> ");
            display.print("2 Players");
            display.display();
        }
        else if (state == 70) {
            if (tttMode == 0 && tttTurn == 2) { // CPU TURN (Minimax Heuristic)
              delay(500); 
              int choice = -1;
              
              // 1. Evaluate immediate Win (2) or Block (1)
              for (int p = 2; p >= 1; p--) {
                if (choice != -1) break;
                for (int i = 0; i < 9; i++) {
                  int r = i / 3, c = i % 3;
                  if (tttGrid[r][c] == 0) {
                    tttGrid[r][c] = p;
                    bool wins = false;
                    for (int j = 0; j < 3; j++) {
                      if (tttGrid[j][0] == p && tttGrid[j][1] == p && tttGrid[j][2] == p) wins = true;
                      if (tttGrid[0][j] == p && tttGrid[1][j] == p && tttGrid[2][j] == p) wins = true;
                    }
                    if (tttGrid[0][0] == p && tttGrid[1][1] == p && tttGrid[2][2] == p) wins = true;
                    if (tttGrid[0][2] == p && tttGrid[1][1] == p && tttGrid[2][0] == p) wins = true;
                    tttGrid[r][c] = 0; 
                    if (wins) { choice = i; break; }
                  }
                }
              }
              
              // 2. Hardware Entropy Fallback
              if (choice == -1) {
                int emptyCells[9], emptyCount = 0;
                for(int i = 0; i < 9; i++) {
                  if(tttGrid[i / 3][i % 3] == 0) { emptyCells[emptyCount++] = i; }
                }
                if(emptyCount > 0) choice = emptyCells[random(emptyCount)];
              }
              
              if (choice != -1) {
                tttGrid[choice / 3][choice % 3] = 2;
                tttTurn = 1;
                checkTTTWin();
              }
              
            } else { // PLAYER TURN OR 2P MODE
              if (digitalRead(btnLeft) == LOW) { tttCursor = (tttCursor > 0) ? tttCursor - 1 : 8; delay(150); }
              if (digitalRead(btnRight) == LOW) { tttCursor = (tttCursor < 8) ? tttCursor + 1 : 0; delay(150); }
              
              if (digitalRead(btnSelect) == LOW) {
                delay(200);
                int cX = tttCursor % 3;
                int cY = tttCursor / 3;
                
                if (tttGrid[cY][cX] == 0) {
                  tttGrid[cY][cX] = tttTurn;
                  tttTurn = (tttTurn == 1) ? 2 : 1;
                  checkTTTWin();
                }
              }
            }
        
            display.clearDisplay();
            int ox = 34;
            int oy = 2;
            display.drawLine(ox + 20, oy, ox + 20, oy + 60, WHITE);
            display.drawLine(ox + 40, oy, ox + 40, oy + 60, WHITE);
            display.drawLine(ox, oy + 20, ox + 60, oy + 20, WHITE);
            display.drawLine(ox, oy + 40, ox + 60, oy + 40, WHITE);
            
            for (int y = 0; y < 3; y++) {
              for (int x = 0; x < 3; x++) {
                int drawX = ox + x * 20 + 10;
                int drawY = oy + y * 20 + 10;
                
                if (tttGrid[y][x] == 1) {
                  display.drawLine(drawX - 6, drawY - 6, drawX + 6, drawY + 6, WHITE);
                  display.drawLine(drawX + 6, drawY - 6, drawX - 6, drawY + 6, WHITE);
                } else if (tttGrid[y][x] == 2) {
                  display.drawCircle(drawX, drawY, 6, WHITE);
                }
                
                // Hide cursor during CPU turn
                if (tttCursor == y * 3 + x && !(tttMode == 0 && tttTurn == 2)) {
                  display.drawRect(drawX - 8, drawY - 8, 17, 17, WHITE);
                }
              }
            }
            
            display.setCursor(0, 0);
            display.print(tttTurn == 1 ? "P1" : (tttMode == 0 ? "CPU" : "P2"));
            display.display();
        }
        else if (state == 71) {
            display.clearDisplay();
            display.setCursor(30, 20);
            
            if (tttWinner == 1) display.print("PLAYER 1 WINS!");
            else if (tttWinner == 2) {
              if (tttMode == 0) display.print("CPU WINS!"); 
              else display.print("PLAYER 2 WINS!"); 
            }
            else display.print("IT'S A TIE!");
            
            display.setCursor(30, 40); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(TicTacToeGame)
