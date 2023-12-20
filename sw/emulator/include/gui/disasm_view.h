#pragma once
#include <gui/component.h>

class Board;

class DisasmView : GuiComponent {
  private:
    Board& board;
  public:
    // vector<string> disasmStrs;
    map<uint32_t, string> disasmStrs;
    DisasmView(Board& board);
    ~DisasmView();
    void update();
};


