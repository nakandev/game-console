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

DisasmView::DisasmView(Board& board)
: board(board)
{
}

DisasmView::~DisasmView()
{
}

void DisasmView::update()
{
  ImGui::Text("%08x", board.cpu.getPc());
  ImGui::BeginChild(ImGui::GetID((void*)10), ImVec2(-1, -1), ImGuiWindowFlags_NoTitleBar);
  for (auto& s: disasmStrs) {
    ImGui::Text("%s", s.second.c_str());
    if (s.first == board.cpu.getPc()) {
      ImGui::SetScrollHereY(0.5);
    }
  }
  ImGui::EndChild();
}
