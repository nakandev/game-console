#pragma once
#include <gui/component.h>

class Board;

class PaletteView : public GuiComponent {
  private:
    Board& board;
  public:
    PaletteView(Board& board);
    ~PaletteView();
    auto update() -> void;
};

