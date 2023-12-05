#pragma once

#include <gui/component.h>
class Board;

class TileView : public GuiComponent {
  private:
    Board& board;
    vector<uint32_t> tiles;
  public:
    TileView(Board& board);
    ~TileView();
    void update();
};

