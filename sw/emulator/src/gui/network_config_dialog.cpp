#include <gui/network_config_dialog.h>
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

NetworkConfigDialog::NetworkConfigDialog(Board& board)
: board(board),
  enable(false)
{
}

NetworkConfigDialog::~NetworkConfigDialog()
{
}

void NetworkConfigDialog::update()
{
  if (enable) {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowSize(ImVec2(300, -1));
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::OpenPopup("Network Config");

    if (ImGui::BeginPopupModal("Network Config")) {
      if (ImGui::Button("OK")) {
        enable = false;
      }
      ImGui::EndPopup();
    }
  }
}
