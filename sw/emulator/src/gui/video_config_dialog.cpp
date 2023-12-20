#include <gui/video_config_dialog.h>
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

VideoConfigDialog::VideoConfigDialog(Board& board)
: board(board),
  windowTheme(),
  screenScale(1), scaleMode(), 
  enable(false)
{
}

VideoConfigDialog::~VideoConfigDialog()
{
}

void VideoConfigDialog::update()
{
  if (enable) {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowSize(ImVec2(300, -1));
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::OpenPopup("Video Config");

    if (ImGui::BeginPopupModal("Video Config")) {
      ImGui::Text("Window theme:");
      ImGui::RadioButton("Dark",  (int*)&windowTheme, static_cast<int>(WINDOW_THEME_DARK));
      ImGui::RadioButton("Light", (int*)&windowTheme, static_cast<int>(WINDOW_THEME_LIGHT));
      ImGui::Text("Screen scale:");
      ImGui::RadioButton("1", &screenScale, 1);
      ImGui::RadioButton("2", &screenScale, 2);
      ImGui::RadioButton("3", &screenScale, 3);
      ImGui::RadioButton("4", &screenScale, 4);
      ImGui::Text("Screen Scale mode:");
      ImGui::RadioButton("Fit",         (int*)&scaleMode, static_cast<int>(SCREEN_SCALE_FIT));
      ImGui::RadioButton("Fit stretch", (int*)&scaleMode, static_cast<int>(SCREEN_SCALE_FIT_STRETCH));
      ImGui::RadioButton("Fit int",     (int*)&scaleMode, static_cast<int>(SCREEN_SCALE_FIT_INT));
      ImGui::RadioButton("Int",         (int*)&scaleMode, static_cast<int>(SCREEN_SCALE_INT));

      if (windowTheme == WINDOW_THEME_LIGHT) {
        ImGui::StyleColorsLight();
      } else {
        ImGui::StyleColorsDark();
      }

      if (ImGui::Button("OK")) {
        enable = false;
      }
      ImGui::EndPopup();
    }
  }
}
