#pragma once
#include <gui/component.h>

class Board;

class MainPanel : GuiComponent {
  private:
    Board& board;
    float scale;
  public:
    MainPanel(Board& board);
    ~MainPanel();
    auto update() -> void;
};

