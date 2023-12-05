#pragma once
#include <filesystem>
#include <gui/main_panel.h>
#include <gui/debug_panel.h>
#include <gui/disasm_view.h>
#include <gui/palette_view.h>
#include <gui/tile_view.h>
#include <gui/mmio_panel.h>

class Board;

class MainComponent {
  private:
    Board& board;
    filesystem::path elfpath;
    filesystem::path elfdir;
    bool guiDebug;

    DebugPanel debugPanel;
    DisasmView disasmView;
    PaletteView paletteView;
    TileView tileView;
    MmioPanel mmioPanel;

    int ctrlTabIndex;
    int viewTabIndex;
    int propertyTabIndex;

    // config
    int screenScale;
    bool fitScale;
    enum {
      SCREEN_SCALE_FIT,
      SCREEN_SCALE_FIT_STRETCH,
      SCREEN_SCALE_FIT_INT,
      SCREEN_SCALE_INT,
    } scaleMode;

    // components size
    float hMenu;
    float margin;
    float thick;
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
    void setElfPath(string path);
    void createFramebuffer();
    void renderMenu();
    void renderMainPanel();
    void renderScreen(int w, int h, bool center);
    void renderFramebuffer();
    void cleanup();
};

void DrawSplitter(int split_vertically, float thickness, float* size0, float* size1, float min_size0, float min_size1);
