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
#include <gui/palette_view.h>
#include <fmt/core.h>

PaletteView::PaletteView(Board& board)
: board(board)
{
}

PaletteView::~PaletteView()
{
}

auto PaletteView::update() -> void
{
  ImGui::Text("Palette View");
  const int bankNum = 4;
  HwTileRam& tileram = *(HwTileRam*)board.memory.section("tile").buffer();
  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  ImVec2 pos00 = ImGui::GetCursorScreenPos();
  int sz = 8;
  for (int palBank=0; palBank<bankNum; palBank++) {
    ImVec2 pos0 = ImVec2(pos00.x + (sz*16+4)*palBank, pos00.y);
    ImVec2 pos0RB = ImVec2(pos0.x + sz*16, pos0.y + sz*16);
    draw_list->AddRect(pos0, pos0RB, 0xff808080, 1);
    for (int y=0; y<16; y++) {
      for (int x=0; x<16; x++) {
        uint32_t c = tileram.palette[palBank].color[x + y * 16].data;
        uint32_t color = c;
        color = (((c>>0) & 0xff) << 24)
              | (((c>>8) & 0xff) << 16)
              | (((c>>16) & 0xff) << 8)
              | (((c>>24) & 0xff) << 0);
        ImVec2 rposLU = ImVec2(pos0.x + sz*x       , pos0.y + sz*y       );
        ImVec2 rposRB = ImVec2(pos0.x + sz*x + sz-1, pos0.y + sz*y + sz-1);
        draw_list->AddRectFilled(rposLU, rposRB, color);
      }
    }
  }
}
