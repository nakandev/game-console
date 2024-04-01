#pragma once
#include <gui/component.h>

class Board;

class TileView : public GuiComponent {
  private:
    Board& board;
    int bankNo;
    int palNo;
    int txn;
    int tyn;
    vector<uint32_t> tiles;
  public:
    GLuint framebuffer;
    GLuint texture;
    GLuint renderbuffer;
    TileView(Board& board);
    ~TileView();
    auto update() -> void;
    auto createFramebuffer() -> void;
    auto renderFramebuffer() -> void;
    auto cleanup() -> void;
};

