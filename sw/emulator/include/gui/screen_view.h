#pragma once
#include <gui/component.h>

class Board;

class ScreenView : GuiComponent {
  private:
    Board& board;
    float scale;
  public:
    ScreenView(Board& board);
    ~ScreenView();
    auto update() -> void;
};
