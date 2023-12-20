#pragma once
#include <gui/component.h>

class Board;

class DebugPanel : public GuiComponent {
  private:
    Board& board;
  public:
    DebugPanel(Board& board);
    ~DebugPanel();
    void update();
    void updateDebugButtons();
    void updateDebugButtons2();
    void updateVideoCtrlPanel();
    void updateAudioCtrlPanel();
    void updateInputCtrlPanel();
};

