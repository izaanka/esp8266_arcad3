#pragma once

class MyNewGame : public ArcadeGame {
private:
  int state = 90;
  // Your game variables here...

public:
  const char *getName() override { return "My New Game"; }

  void init() override {
    // Your setup code here...
  }

  void update() override {
    // Your loop code here...
    // Remember to call returnToMenu(); when you need to go back!
  }
};

REGISTER_GAME(MyNewGame)
