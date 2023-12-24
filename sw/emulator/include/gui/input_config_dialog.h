#pragma once
#include <gui/component.h>
#include <string>
#include <vector>
#include <SDL2/SDL.h>

using namespace std;

class Board;

struct PadInfo {
  SDL_GameController* device;
  int deviceId;
  string name;
  vector<int> keyAsigns;
};

class InputConfigDialog : GuiComponent {
  private:
    Board& board;
    int phase;
  public:
    bool enable;
    string inputConfigPath;
    PadInfo padInfo;
    InputConfigDialog(Board& board);
    ~InputConfigDialog();
    auto update() -> void;
    auto draw() -> void;
    auto selectPadDevice() -> void;
    auto setupPadKey() -> void;
    auto setupPadKeyAll() -> void;
};

