#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#define okCancelButtonAlignement 1.0f
#include "ImGuiFileDialog.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SDL2/SDL_opengl.h>
#include <emulator.h>
#include <gui.h>
#include <fmt/core.h>

TileView::TileView(Board& board)
: board(board)
{
}

TileView::~TileView()
{
}

void TileView::update()
{
  HwTileRam& tileram = *(HwTileRam*)board.memory.section("tile").buffer();
  // int palNo = 1;
  // for (int palNo=0; palNo<palNum; palNo++) {
  //   for (int i=0; i<sizeof(colors)/sizeof(colors[0]); i++) {
  //     colors[palNo][i] = tileram.palette[palNo].color[i].data;
  //   }
  // }
}
