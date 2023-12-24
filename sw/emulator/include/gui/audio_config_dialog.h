#pragma once
#include <gui/component.h>

class Board;

class AudioConfigDialog : GuiComponent {
  private:
    Board& board;
  public:
    bool enable;
    int soundDeviceId;
    AudioConfigDialog(Board& board);
    ~AudioConfigDialog();
    auto update() -> void;
    auto selectSoundDevice() -> void;
};

