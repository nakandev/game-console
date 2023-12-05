#pragma once

#include <gui/component.h>
class Board;

class NetworkConfigDialog : GuiComponent {
  private:
    Board* board;
    float scale;
  public:
    NetworkConfigDialog();
    ~NetworkConfigDialog();
    void update();
};

