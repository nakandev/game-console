#pragma once
#include <gui/component.h>

class Board;

class MmioPanel : public GuiComponent {
  private:
    Board& board;
  public:
    MmioPanel(Board& board);
    ~MmioPanel();
    void update();
};

