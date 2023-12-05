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

void DebugPanel::update()
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

void DebugPanel::updateDebugButtons()
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
void DebugPanel::updateDebugButtons2()
{
  ImGui::Text("Debugger\n");
  ImGui::Button("si"); ImGui::SameLine();  // Step CPU Instruction
  ImGui::Button("st"); ImGui::SameLine();  // Step through
  ImGui::Button("so"); ImGui::SameLine();  // Step out
  ImGui::Button("sv"); // Step until VSYNC
  ImGui::Button("breakpoint");
}
void DebugPanel::updateVideoCtrlPanel()
{
  ImGui::Text("Video\n");
  ImGui::Checkbox("BG0", &board.ppu.debug.enableBg[0].v1); ImGui::SameLine();
  ImGui::Checkbox("BG1", &board.ppu.debug.enableBg[1].v1); ImGui::SameLine();
  ImGui::Checkbox("BG2", &board.ppu.debug.enableBg[2].v1); ImGui::SameLine();
  ImGui::Checkbox("BG3", &board.ppu.debug.enableBg[3].v1);
  ImGui::Checkbox("SP ", &board.ppu.debug.enableSp.v1);
}
void DebugPanel::updateAudioCtrlPanel()
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
void DebugPanel::updateInputCtrlPanel()
{
  ImGui::Text("Input\n");
  HwPad hwpad = board.io.getPadStatus();
  bool A = hwpad.A, B = hwpad.B, C = hwpad.C, D = hwpad.D;
  bool L = hwpad.L, R = hwpad.R, S = hwpad.S, T = hwpad.T;
  bool Up = hwpad.UP, Dw = hwpad.DOWN, Lf = hwpad.LEFT, Rt = hwpad.RIGHT;
  ImGui::Checkbox("A", &A); ImGui::SameLine();
  ImGui::Checkbox("B", &B); ImGui::SameLine();
  ImGui::Checkbox("C", &C); ImGui::SameLine();
  ImGui::Checkbox("D", &D);
  ImGui::Checkbox("L", &L); ImGui::SameLine();
  ImGui::Checkbox("R", &R); ImGui::SameLine();
  ImGui::Checkbox("S", &S); ImGui::SameLine();
  ImGui::Checkbox("T", &T);
  ImGui::Checkbox("Up", &Up); ImGui::SameLine();
  ImGui::Checkbox("Dw", &Dw); ImGui::SameLine();
  ImGui::Checkbox("Lf", &Lf); ImGui::SameLine();
  ImGui::Checkbox("Rt", &Rt);
}
