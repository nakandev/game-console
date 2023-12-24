#pragma once
#include <filesystem>
#include <gui/main_panel.h>
#include <gui/debug_panel.h>
#include <gui/disasm_view.h>
#include <gui/palette_view.h>
#include <gui/tile_view.h>
#include <gui/mmio_panel.h>
#include <gui/input_config_dialog.h>
#include <gui/video_config_dialog.h>
#include <gui/audio_config_dialog.h>
#include <gui/network_config_dialog.h>

class Board;

class MainComponent {
  public:
    Board& board;
  private:
    filesystem::path elfpath;
    filesystem::path elfdir;
    bool guiDebug;

    DebugPanel debugPanel;
    DisasmView disasmView;
    PaletteView paletteView;
    TileView tileView;
    MmioPanel mmioPanel;
  public:
    InputConfigDialog inputConfigDialog;
    VideoConfigDialog videoConfigDialog;
    AudioConfigDialog audioConfigDialog;
    NetworkConfigDialog networkConfigDialog;
  private:
    int ctrlTabIndex;
    int viewTabIndex;
    int propertyTabIndex;

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
    auto setElfPath(string path) -> void;
    auto createFramebuffer() -> void;
    auto renderMenu() -> void;
    auto renderMainPanel() -> void;
    auto renderScreen(int w, int h, bool center) -> void;
    auto renderFramebuffer() -> void;
    auto cleanup() -> void;
};

auto DrawSplitter(int split_vertically, float thickness, float* size0, float* size1, float min_size0, float min_size1) -> void;
