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
#include <gui/debug_panel.h>
#include <fmt/core.h>

DebugPanel::DebugPanel(Board& board)
: board(board)
{
}

DebugPanel::~DebugPanel()
{
}

auto DebugPanel::update() -> void
{
  ImGui::Text("Control Panel\n");
  ImGui::Separator();
  updateDebugButtons();
  ImGui::Separator();
  updateDebugButtons2();
  ImGui::Separator();
  updateVideoCtrlPanel();
  ImGui::Separator();
  updateAudioCtrlPanel();
  ImGui::Separator();
  updateInputCtrlPanel();
}

auto DebugPanel::updateDebugButtons() -> void
{
  ImGui::Text("Play\n");
  ImGui::Button(">"); ImGui::SameLine();  // Run/Pause
  ImGui::Button(">>"); ImGui::SameLine();  // Boost
  ImGui::Button("<<"); ImGui::SameLine();  // Reverse
  ImGui::Button("R"); ImGui::SameLine();  // Reset
  ImGui::Button("Q");  // Power off
  ImGui::Button("Save state");
  ImGui::Button("Load state");
}

auto DebugPanel::updateDebugButtons2() -> void
{
  ImGui::Text("Debugger\n");
  ImGui::Button("si"); ImGui::SameLine();  // Step CPU Instruction
  ImGui::Button("st"); ImGui::SameLine();  // Step through
  ImGui::Button("so"); ImGui::SameLine();  // Step out
  ImGui::Button("sv"); // Step until VSYNC
  ImGui::Button("breakpoint");
}

auto DebugPanel::updateVideoCtrlPanel() -> void
{
  ImGui::Text("Video\n");
  ImGui::Checkbox("BG0", &board.vpu.debug.enableBg[0].v1); ImGui::SameLine();
  ImGui::Checkbox("BG1", &board.vpu.debug.enableBg[1].v1); ImGui::SameLine();
  ImGui::Checkbox("BG2", &board.vpu.debug.enableBg[2].v1); ImGui::SameLine();
  ImGui::Checkbox("BG3", &board.vpu.debug.enableBg[3].v1);
  ImGui::Checkbox("SP ", &board.vpu.debug.enableSp.v1);
}

auto DebugPanel::updateAudioCtrlPanel() -> void
{
  ImGui::Text("Audio\n");
  ImGui::Checkbox("Ch0", &board.apu.debug.enableCh[0 ].v1); ImGui::SameLine();
  ImGui::Checkbox("Ch1", &board.apu.debug.enableCh[1 ].v1); ImGui::SameLine();
  ImGui::Checkbox("Ch2", &board.apu.debug.enableCh[2 ].v1); ImGui::SameLine();
  ImGui::Checkbox("Ch3", &board.apu.debug.enableCh[3 ].v1);
  ImGui::Checkbox("Ch4", &board.apu.debug.enableCh[4 ].v1); ImGui::SameLine();
  ImGui::Checkbox("Ch5", &board.apu.debug.enableCh[5 ].v1); ImGui::SameLine();
  ImGui::Checkbox("Ch6", &board.apu.debug.enableCh[6 ].v1); ImGui::SameLine();
  ImGui::Checkbox("Ch7", &board.apu.debug.enableCh[7 ].v1);
}

auto DebugPanel::updateInputCtrlPanel() -> void
{
  ImGui::Text("Input\n");
  HwPad hwpad = board.io.getPadStatus();
  bool A = hwpad.A, B = hwpad.B, C = hwpad.C, D = hwpad.D;
  bool L = hwpad.L, R = hwpad.R, S = hwpad.S, T = hwpad.T;
  bool Up = hwpad.UP, Dw = hwpad.DOWN, Lf = hwpad.LEFT, Rt = hwpad.RIGHT;
  ImVec2 pos0 = ImGui::GetCursorScreenPos();
  auto setButton = [&](int x, int y, const char* s, bool on) {
    auto bgColor = ImVec4(0.5, 0.5, 0.5, 1.0);
    if (on) bgColor = ImVec4(0.2, 0.2, 0.8, 1.0);
    ImGui::PushStyleColor(ImGuiCol_Button, bgColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, bgColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, bgColor);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));
    ImGui::SetCursorPos(ImVec2(pos0.x+x, pos0.y+y)); ImGui::Button(s);
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
  };
  setButton(  0,  0, " L  ", L);
  setButton(120,  0, "  R ", R);
  setButton( 20, 20, "Up", Up);
  setButton(  0, 40, "Lf", Lf);
  setButton( 40, 40, "Rt", Rt);
  setButton( 20, 60, "Dw", Dw);
  setButton(120, 20, "C", C);
  setButton(100, 40, "D", D);
  setButton(140, 40, "A", A);
  setButton(120, 60, "B", B);
  setButton( 60, 70, "S", S);
  setButton( 80, 70, "T", T);
}
