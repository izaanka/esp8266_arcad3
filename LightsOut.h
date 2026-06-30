#pragma once

class LightsOutGame : public ArcadeGame {
private:
    const bool inv3x3[9][9] = {
      {1, 0, 1, 0, 0, 1, 1, 1, 0}, {0, 0, 0, 0, 1, 0, 1, 1, 1}, {1, 0, 1, 1, 0, 0, 0, 1, 1},
      {0, 0, 1, 0, 1, 1, 0, 0, 1}, {0, 1, 0, 1, 1, 1, 0, 1, 0}, {1, 0, 0, 1, 1, 0, 1, 0, 0},
      {1, 1, 0, 0, 0, 1, 1, 0, 1}, {1, 1, 1, 0, 1, 0, 0, 0, 0}, {0, 1, 1, 1, 0, 0, 1, 0, 1}
    };
    bool grid[6][12]; 
    int cursorIndex = 0; 
    bool showHint = false;
    int loMenuOption = 0;
    int gridW = 3, gridH = 3, totalTiles = 9;
    int spacing = 20, boxSize = 18, lightOffset = 4, lightSize = 10;
    int state = 10;

    void toggleTile(int x, int y) {
      if (x >= 0 && x < gridW && y >= 0 && y < gridH) { grid[y][x] = !grid[y][x]; }
    }
    void pressButton(int x, int y) {
      toggleTile(x, y); toggleTile(x - 1, y); toggleTile(x + 1, y);
      toggleTile(x, y - 1); toggleTile(x, y + 1);
    }
    void generatePuzzle() {
      for (int y = 0; y < gridH; y++) { for (int x = 0; x < gridW; x++) { grid[y][x] = false; } }
      int randomMoves = random(totalTiles * 2, totalTiles * 5);
      for (int i = 0; i < randomMoves; i++) { pressButton(random(gridW), random(gridH)); }
      cursorIndex = 0; state = 11;
      showHint = false;
    }

public:
    const char* getName() override { return "Lights Out"; }
    void init() override {
        loMenuOption = 0;
        state = 10;
    }
    
    void update() override {
        if (state == 10) { 
            if (digitalRead(btnLeft) == LOW) { loMenuOption = (loMenuOption > 0) ? loMenuOption - 1 : 2; delay(150); }
            if (digitalRead(btnRight) == LOW) { loMenuOption = (loMenuOption < 2) ? loMenuOption + 1 : 0; delay(150); }
            if (digitalRead(btnSelect) == LOW) {
              if (loMenuOption == 0) { gridW = 3; gridH = 3; spacing = 20; boxSize = 18; lightOffset = 4; lightSize = 10; }
              else if (loMenuOption == 1) { gridW = 5; gridH = 5; spacing = 12; boxSize = 10; lightOffset = 2; lightSize = 6; }
              else if (loMenuOption == 2) { gridW = 12; gridH = 6; spacing = 10; boxSize = 8; lightOffset = 2; lightSize = 4; }
              totalTiles = gridW * gridH;
              generatePuzzle();
              delay(200);
            }
            display.clearDisplay();
            display.setCursor(10, 10);
            display.print("Select Grid Size:");
            const char* options[] = {"3x3 (Easy)", "5x5 (Normal)", "12x6 (Extreme)"};
            for(int i = 0; i < 3; i++) {
              display.setCursor(20, 30 + (i * 10));
              if(i == loMenuOption) display.print("> ");
              display.print(options[i]);
            }
            display.display();
        } 
        else if (state == 11) { 
            if (digitalRead(btnLeft) == LOW) { cursorIndex = (cursorIndex > 0) ? cursorIndex - 1 : totalTiles - 1; delay(120); }
            if (digitalRead(btnRight) == LOW) { cursorIndex = (cursorIndex < totalTiles - 1) ? cursorIndex + 1 : 0; delay(120); }
            if (digitalRead(btnSelect) == LOW) {
              int holdTime = 0;
              while (digitalRead(btnSelect) == LOW && holdTime < 10) { delay(50); holdTime++; }
              if (holdTime >= 10 && gridW == 3) { showHint = !showHint; } 
              else {
                int cX = cursorIndex % gridW;
                int cY = cursorIndex / gridW;
                pressButton(cX, cY); showHint = false;
              }
              while (digitalRead(btnSelect) == LOW) { delay(10); }
              delay(50);
              
              bool allOff = true;
              for (int y = 0; y < gridH; y++) {
                for (int x = 0; x < gridW; x++) { if (grid[y][x] == true) allOff = false; }
              }
              if (allOff) state = 12;
            }
        
            display.clearDisplay();
            int offsetX = (128 - (gridW * spacing)) / 2;
            int offsetY = (64 - (gridH * spacing)) / 2;
            
            for (int y = 0; y < gridH; y++) {
              for (int x = 0; x < gridW; x++) {
                int drawX = offsetX + (x * spacing);
                int drawY = offsetY + (y * spacing);
                display.drawRect(drawX, drawY, boxSize, boxSize, WHITE);
                
                if (grid[y][x]) { display.fillRect(drawX + lightOffset, drawY + lightOffset, lightSize, lightSize, WHITE); }
                if (cursorIndex == (y * gridW + x)) { display.drawRect(drawX - 2, drawY - 2, boxSize + 4, boxSize + 4, WHITE); }
                
                if (showHint && gridW == 3) {
                  bool needsPress = false;
                  int tileIndex = y * 3 + x;
                  for (int i = 0; i < 9; i++) {
                    if (grid[i / 3][i % 3] && inv3x3[tileIndex][i]) needsPress = !needsPress;
                  }
                  if (needsPress) {
                    display.drawLine(drawX, drawY, drawX + boxSize, drawY + boxSize, WHITE);
                    display.drawLine(drawX + boxSize, drawY, drawX, drawY + boxSize, WHITE);
                  }
                }
              }
            }
            display.display();
        } 
        else if (state == 12) { 
            display.clearDisplay();
            display.setCursor(25, 20); display.print("PUZZLE BEATEN!");
            display.setCursor(25, 40); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(LightsOutGame)
