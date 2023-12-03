#pragma once
#include <gui/component.h>
class Board;

class ScreenView : GuiComponent {
  private:
    Board* board;
    float scale;
  public:
    ScreenView();
    ~ScreenView();
    void update();
};
