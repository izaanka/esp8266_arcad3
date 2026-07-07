#pragma once

class MorseTranslator : public ArcadeGame {
private:
  unsigned long pressStartTime = 0;
  unsigned long releaseStartTime = 0;
  bool isPressed = false;
  bool processingChar = false;
  bool spaceAdded = true;
  
  const int dotMaxDuration = 200;   
  const int dashMaxDuration = 1000;  
  const int charGap = 1400;         
  const int wordGap = 5000; 
  
  String currentMorse = "";
  String displayBuffer = ""; 
  
  const String letters[26] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", 
    ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", 
    "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." 
  };
  const String numbers[10] = {
    "-----", ".----", "..---", "...--", "....-", 
    ".....", "-....", "--...", "---..", "----."  
  };

  void decodeAndPrint(String morseSequence) {
    char decodedChar = '?'; 
    for (int i = 0; i < 26; i++) {
      if (letters[i] == morseSequence) {
        decodedChar = 'A' + i;
        break;
      }
    }
    if (decodedChar == '?') {
      for (int i = 0; i < 10; i++) {
        if (numbers[i] == morseSequence) {
          decodedChar = '0' + i;
          break;
        }
      }
    }
    processNewCharacter(decodedChar);
  }

  void processNewCharacter(char c) {
    displayBuffer += c;
    if (displayBuffer.length() > 42) {
      displayBuffer = displayBuffer.substring(displayBuffer.length() - 42);
    }
  }

public:
  const char* getName() override { return "Morse Translator"; }
  const char* getCategory() override { return "Games"; }

  void init() override {
    currentMorse = "";
    displayBuffer = "";
    isPressed = false;
    processingChar = false;
    spaceAdded = true;
    pressStartTime = 0;
    releaseStartTime = millis();
  }

  void update() override {
    if (digitalRead(btnLeft) == LOW) {
      delay(200);
      returnToMenu();
      return;
    }

    bool currentSensorState = (digitalRead(btnSelect) == LOW);
    unsigned long currentTime = millis();

    if (currentSensorState && !isPressed) {
      isPressed = true;
      pressStartTime = currentTime;
      spaceAdded = false;
    }
    else if (!currentSensorState && isPressed) {
      isPressed = false;
      releaseStartTime = currentTime;
      
      unsigned long pressDuration = currentTime - pressStartTime;
      
      if (pressDuration >= 200 && pressDuration <= dashMaxDuration) {
        currentMorse += "-";
        processingChar = true;
      } else if (pressDuration >= 20 && pressDuration < dotMaxDuration) {
        currentMorse += ".";
        processingChar = true;
      }
    }

    if (!isPressed && processingChar) {
      if ((currentTime - releaseStartTime) >= charGap) {
        if (currentMorse.length() > 0) {
          decodeAndPrint(currentMorse);
        }
        currentMorse = "";           
        processingChar = false;      
      }
    }

    if (!isPressed && !processingChar && releaseStartTime > 0) {
      if (!spaceAdded && (currentTime - releaseStartTime) >= wordGap) {
        processNewCharacter(' ');             
        spaceAdded = true;           
      }
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    display.setCursor(0, 0);
    display.print("Morse Translator");

    if (isPressed) {
        display.fillRect(115, 0, 10, 10, WHITE);
    } else {
        display.drawRect(115, 0, 10, 10, WHITE);
    }
    
    display.setCursor(0, 15);
    display.print("Input: ");
    display.print(currentMorse);
    
    display.setCursor(0, 30);
    if (displayBuffer.length() > 21) {
        display.print(displayBuffer.substring(0, 21));
        display.setCursor(0, 40);
        display.print(displayBuffer.substring(21));
    } else {
        display.print(displayBuffer);
    }
    
    display.setCursor(0, 56);
    display.print("< Exit");

    display.display();
    delay(10);
  }
};

REGISTER_GAME(MorseTranslator)
