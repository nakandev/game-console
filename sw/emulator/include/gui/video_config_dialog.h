#pragma once

#include <gui/component.h>
class Board;

class VideoConfigDialog : GuiComponent {
  private:
    Board* board;
    float scale;
  public:
    VideoConfigDialog();
    ~VideoConfigDialog();
    void update();
};

