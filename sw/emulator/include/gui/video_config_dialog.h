#pragma once
#include <gui/component.h>

class Board;

class VideoConfigDialog : GuiComponent {
  private:
    Board& board;
  public:
    enum {
      WINDOW_THEME_DARK,
      WINDOW_THEME_LIGHT,
    } windowTheme;
    int screenScale;
    enum {
      SCREEN_SCALE_FIT,
      SCREEN_SCALE_FIT_STRETCH,
      SCREEN_SCALE_FIT_INT,
      SCREEN_SCALE_INT,
    } scaleMode;
  public:
    bool enable;
    VideoConfigDialog(Board& board);
    ~VideoConfigDialog();
    auto update() -> void;
};

