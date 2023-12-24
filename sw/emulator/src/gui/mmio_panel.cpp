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
#include <gui/mmio_panel.h>
#include <fmt/core.h>

MmioPanel::MmioPanel(Board& board)
: board(board)
{
}

MmioPanel::~MmioPanel()
{
}

auto MmioPanel::update() -> void
{
  ImGui::Text("MMIO Panel\n");
}
