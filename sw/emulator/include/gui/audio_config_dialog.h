#pragma once

#include <gui/component.h>
class Board;

class AudioConfigDialog : GuiComponent {
  private:
    Board* board;
  public:
    int soundDeviceId;
    AudioConfigDialog();
    ~AudioConfigDialog();
    void update();
    void selectSoundDevice();
};

