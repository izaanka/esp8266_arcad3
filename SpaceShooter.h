#pragma once

class SpaceShooterGame : public ArcadeGame {
private:
    int shipX = 60;
    int laserX = -1, laserY = -1;
    bool laserActive = false;
    int alienX = 0, alienY = 0, alienDir = 2;
    int shooterScore = 0;
    int state = 60;

public:
    const char* getName() override { return "Space Shooter"; }

    void init() override {
        shipX = 60;
        laserActive = false;
        alienX = random(10, 110);
        alienY = 0;
        alienDir = 2;
        shooterScore = 0;
        state = 60;
    }

    void update() override {
        if (state == 60) {
            if (digitalRead(btnLeft) == LOW) shipX -= 4;
            if (digitalRead(btnRight) == LOW) shipX += 4;
            
            if (shipX < 0) shipX = 0;
            if (shipX > 116) shipX = 116;
        
            if (digitalRead(btnSelect) == LOW && !laserActive) {
              laserActive = true;
              laserX = shipX + 5;
              laserY = 54;
            }
        
            alienX += alienDir;
            if (alienX > 118 || alienX < 0) {
              alienDir = -alienDir;
              alienY += 6;
            }
        
            if (laserActive) {
              laserY -= 6;
              if (laserY < 0) laserActive = false;
              
              if (laserX >= alienX && laserX <= alienX + 10 && laserY <= alienY + 8 && laserY >= alienY) {
                laserActive = false;
                shooterScore++;
                alienY = 0;
                alienX = random(10, 110);
                alienDir = (alienDir > 0) ? alienDir + 1 : alienDir - 1; 
              }
            }
        
            if (alienY > 50) {
              state = 61;
              delay(500);
            }
        
            display.clearDisplay();
            display.fillRect(shipX, 58, 12, 6, WHITE); // Ship
            display.fillRect(shipX + 4, 54, 4, 4, WHITE); // Ship Turret
            display.fillRect(alienX, alienY, 10, 8, WHITE); // Alien
            
            if (laserActive) {
              display.drawLine(laserX, laserY, laserX, laserY + 4, WHITE);
            }
            
            display.setCursor(0, 0); 
            display.print(shooterScore);
            display.display();
            delay(15);
        }
        else if (state == 61) {
            display.clearDisplay();
            display.setCursor(20, 20); display.print("EARTH INVADED!");
            display.setCursor(20, 35); display.print("Score: ");
            display.print(shooterScore);
            display.setCursor(20, 50); display.print("D6: Main Menu");
            if (digitalRead(btnSelect) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(SpaceShooterGame)
