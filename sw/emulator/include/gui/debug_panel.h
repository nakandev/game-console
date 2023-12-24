#pragma once
#include <gui/component.h>

class Board;

class DebugPanel : public GuiComponent {
  private:
    Board& board;
  public:
    DebugPanel(Board& board);
    ~DebugPanel();
    auto update() -> void;
    auto updateDebugButtons() -> void;
    auto updateDebugButtons2() -> void;
    auto updateVideoCtrlPanel() -> void;
    auto updateAudioCtrlPanel() -> void;
    auto updateInputCtrlPanel() -> void;
};

