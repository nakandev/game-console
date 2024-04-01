#pragma once
#include <gui/component.h>

class Board;

class TilemapView : public GuiComponent {
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
    TilemapView(Board& board);
    ~TilemapView();
    auto update() -> void;
    auto createFramebuffer() -> void;
    auto renderFramebuffer() -> void;
    auto cleanup() -> void;
};

