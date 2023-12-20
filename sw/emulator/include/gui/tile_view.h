#pragma once
#include <gui/component.h>

class Board;

class TileView : public GuiComponent {
  private:
    Board& board;
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
    void update();
    void createFramebuffer();
    void renderFramebuffer();
    void cleanup();
};

