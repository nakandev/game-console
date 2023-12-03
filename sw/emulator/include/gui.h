#pragma once
#include <filesystem>
#include <gui/main_panel.h>

class Board;

class MainComponent {
  private:
    Board& board;
    filesystem::path elfpath;
    filesystem::path elfdir;
    bool guiDebug;

    // temporary
    int screenScale;
    // 800 x 600
    float hMenu;
    float margin;
    float thick;
    // float wSCR = io.DisplaySize.x, hSCR = io.DisplaySize.y;
    float wSCR, hSCR;
    float wLC, hLC;
    float wL, hL;
    float wC, hC;
    float wR, hR;
    float wB, hB;
  public:
    GLuint framebuffer;
    GLuint texture;
    GLuint renderbuffer;
    vector<uint32_t> screenBuffer;

    MainComponent(Board& board);
    ~MainComponent();
    void createFramebuffer();
    void renderMenu();
    void renderMainPanel();
    void renderFramebuffer();
    void cleanup();
};

void DrawSplitter(int split_vertically, float thickness, float* size0, float* size1, float min_size0, float min_size1);
