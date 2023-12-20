#include <gui/input_config_dialog.h>
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

InputConfigDialog::InputConfigDialog(Board& board)
: board(board), phase(),
  enable(false),
  inputConfigPath(),
  padInfo()
{
  padInfo.keyAsigns.resize(16);
  padInfo.deviceId = 0;
  padInfo.keyAsigns[ 0] = SDL_CONTROLLER_BUTTON_A;
  padInfo.keyAsigns[ 1] = SDL_CONTROLLER_BUTTON_B;
  padInfo.keyAsigns[ 2] = SDL_CONTROLLER_BUTTON_X;
  padInfo.keyAsigns[ 3] = SDL_CONTROLLER_BUTTON_Y;
  padInfo.keyAsigns[ 4] = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
  padInfo.keyAsigns[ 5] = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
  padInfo.keyAsigns[ 6] = SDL_CONTROLLER_BUTTON_BACK;
  padInfo.keyAsigns[ 7] = SDL_CONTROLLER_BUTTON_START;
  padInfo.keyAsigns[12] = SDL_CONTROLLER_BUTTON_DPAD_UP;
  padInfo.keyAsigns[13] = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
  padInfo.keyAsigns[14] = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
  padInfo.keyAsigns[15] = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;

  padInfo.device = SDL_GameControllerOpen(padInfo.deviceId);
  if (padInfo.device) {
    auto* joystick = SDL_JoystickOpen(padInfo.deviceId);
    padInfo.name = SDL_JoystickName(joystick);
    SDL_JoystickClose(joystick);
  }
}

InputConfigDialog::~InputConfigDialog()
{
  if (padInfo.device) SDL_GameControllerClose(padInfo.device);
}

void InputConfigDialog::update()
{
  if (enable) {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowSize(ImVec2(300, -1));
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::OpenPopup("Input Config");

    if (ImGui::BeginPopupModal("Input Config", nullptr, ImGuiWindowFlags_NoNav)) {
      auto devices = vector<PadInfo>();
      for (int i = 0, joystick_count = SDL_NumJoysticks(); i < joystick_count; i++) {
        auto* joystick = SDL_JoystickOpen(i);
        if (!joystick) continue;
        auto* name = SDL_JoystickName(joystick);
        devices.push_back({nullptr, i, name, vector<int>(12, 0)});
        SDL_JoystickClose(joystick);
      }

      auto selectedDeviceName = fmt::format("{}:{}", padInfo.deviceId, padInfo.name);
      if (ImGui::BeginCombo("Device", selectedDeviceName.c_str())) {
        for (auto& device: devices) {
          auto deviceName = fmt::format("{}:{}", device.deviceId, device.name);
          if (ImGui::Selectable(deviceName.c_str())) {
            padInfo.deviceId = device.deviceId;
            padInfo.name = device.name;
            if (padInfo.device) SDL_GameControllerClose(padInfo.device);
            padInfo.device = SDL_GameControllerOpen(padInfo.deviceId);
            // load padInfo.keyAsigns from name
          }
        }
        ImGui::EndCombo();
      }
      ImGui::BeginChild(ImGui::GetID((void*)100), ImVec2(-1, 200), ImGuiWindowFlags_NoTitleBar);
      ImGui::Button("A "); ImGui::SameLine(); ImGui::InputInt("A ", &padInfo.keyAsigns[ 0]);
      ImGui::Button("B "); ImGui::SameLine(); ImGui::InputInt("B ", &padInfo.keyAsigns[ 1]);
      ImGui::Button("C "); ImGui::SameLine(); ImGui::InputInt("C ", &padInfo.keyAsigns[ 2]);
      ImGui::Button("D "); ImGui::SameLine(); ImGui::InputInt("D ", &padInfo.keyAsigns[ 3]);
      ImGui::Button("L "); ImGui::SameLine(); ImGui::InputInt("L ", &padInfo.keyAsigns[ 4]);
      ImGui::Button("R "); ImGui::SameLine(); ImGui::InputInt("R ", &padInfo.keyAsigns[ 5]);
      ImGui::Button("S "); ImGui::SameLine(); ImGui::InputInt("S ", &padInfo.keyAsigns[ 6]);
      ImGui::Button("T "); ImGui::SameLine(); ImGui::InputInt("T ", &padInfo.keyAsigns[ 7]);
      ImGui::Button("Up"); ImGui::SameLine(); ImGui::InputInt("Up", &padInfo.keyAsigns[12]);
      ImGui::Button("Dw"); ImGui::SameLine(); ImGui::InputInt("Dw", &padInfo.keyAsigns[13]);
      ImGui::Button("Lf"); ImGui::SameLine(); ImGui::InputInt("Lf", &padInfo.keyAsigns[14]);
      ImGui::Button("Rt"); ImGui::SameLine(); ImGui::InputInt("Rt", &padInfo.keyAsigns[15]);
      ImGui::EndChild();
      if (ImGui::Button("OK")) {
        enable = false;
      }
      ImGui::EndPopup();
    }
  }
}

void InputConfigDialog::draw()
{
}

void InputConfigDialog::selectPadDevice()
{
}

void InputConfigDialog::setupPadKey()
{
}

void InputConfigDialog::setupPadKeyAll()
{
}
