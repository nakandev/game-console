#pragma once
#include <gui/component.h>

class Board;

class NetworkConfigDialog : GuiComponent {
  private:
    Board& board;
  public:
    bool enable;
    NetworkConfigDialog(Board& board);
    ~NetworkConfigDialog();
    auto update() -> void;
};

