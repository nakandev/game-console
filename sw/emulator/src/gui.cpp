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

MainComponent::MainComponent(Board& board)
: board(board),
  elfpath(),
  elfdir(),
  guiDebug(false)
{
  elfpath = "/home/nyalry/nakan/dev/hobby/game-console/sw/dev/c/test/trial_08_tilefile/trial_08_tilefile";
  screenBuffer.resize(320 * 240);
  // 800 x 600
  screenScale = 3;
  hMenu = 30;
  margin = 10;
  thick = 10;
  // float wSCR = io.DisplaySize.x, hSCR = io.DisplaySize.y;
  wSCR = HW_SCREEN_W * screenScale - margin, hSCR = HW_SCREEN_H * screenScale - margin - hMenu;
  wLC = (wSCR*0.8)-thick, hLC = (hSCR*0.8)-thick;
  wL = (wSCR*0.2)-thick, hL = hLC;
  wC = (wSCR*0.6)-thick, hC = hLC;
  wR = (wSCR*0.2), hR = (hSCR*1.0);
  wB = (wSCR*0.8)-thick, hB = (hSCR*0.2);
}

MainComponent::~MainComponent()
{
}

void MainComponent::createFramebuffer()
{
  int width = HW_SCREEN_W, height = HW_SCREEN_H;
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

  glGenRenderbuffers(1, &renderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    fmt::print("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void MainComponent::renderMenu()
{
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("System")) {
      if(ImGui::MenuItem("Open", "CTRL+O")) {
        string dir = elfpath;
        ImGuiFileDialog::Instance()->OpenDialog(
          "ChooseFileDlgKey", "Choose File", ".exe,.elf,.out", dir,
          1, nullptr, ImGuiFileDialogFlags_Modal
        );
      }

      ImGui::Separator();
      ImGui::MenuItem("Quit", "CTRL+Q");
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Config")) {
      ImGui::MenuItem("Input", "");
      ImGui::MenuItem("Video", "");
      ImGui::MenuItem("Audio", "");
      ImGui::MenuItem("Network", "");
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Emulator")) {
      ImGui::BeginDisabled(!board.pause);
      bool retRun = ImGui::MenuItem("Run", "");
      ImGui::EndDisabled();
      ImGui::BeginDisabled(board.pause);
      bool retPause = ImGui::MenuItem("Pause", "");
      ImGui::EndDisabled();
      if (retRun) {
        board.pause = false;
      }
      if (retPause) {
        board.pause = true;
      }
      if (ImGui::MenuItem("Reset", "")) {
        board.reset();
        board.cpu.loadElf(elfpath);
      }
      ImGui::Separator();
      ImGui::MenuItem("Debug Mode", "", &guiDebug);
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
  /* File Dialog */
  if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) 
  {
    if (ImGuiFileDialog::Instance()->IsOk())
    {
      string selectedFile = ImGuiFileDialog::Instance()->GetFilePathName();
      string selectedDir = ImGuiFileDialog::Instance()->GetCurrentPath();
      elfpath = selectedFile;
      elfdir = selectedDir;
      board.cpu.init();
      if(!board.cpu.loadElf(elfpath)) {
        // disasms = board.cpu.disassembleAll();
        board.pause = false;
      }
    }
    ImGuiFileDialog::Instance()->Close();
  }
}

void MainComponent::renderMainPanel()
{
  ImVec2 winsize = ImGui::GetWindowSize();
  // fmt::print("winW={}, winH={}\n", (int)winsize.x, (int)winsize.y);
  wSCR = winsize.x - margin, hSCR = winsize.y - margin - hMenu;
  wLC = wSCR - wR - thick;
  hLC = hSCR - hB - thick;
  hR = hSCR;
  hC = hLC;
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
  DrawSplitter(false, 10, &wLC, &wR, 50, 50);
  wC = wLC - wL - thick;
  wB = wLC;
  ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(wLC, hR), ImGuiWindowFlags_NoTitleBar);
    DrawSplitter(true, 10, &hLC, &hB, 50, 50);
    ImGui::BeginChild(ImGui::GetID((void*)1), ImVec2(wLC, hLC), ImGuiWindowFlags_NoTitleBar);
      DrawSplitter(false, 10, &wL, &wC, 50, 50);
      ImGui::BeginChild(ImGui::GetID((void*)2), ImVec2(wL, hLC), ImGuiWindowFlags_NoTitleBar);
      {
        ImGui::Text("Control Panel\n");
        ImGui::Separator();
        ImGui::Text("Video\n");
        ImGui::Checkbox("BG0", &board.ppu.debug.enableBg[0].v1); ImGui::SameLine();
        ImGui::Checkbox("BG1", &board.ppu.debug.enableBg[1].v1); ImGui::SameLine();
        ImGui::Checkbox("BG2", &board.ppu.debug.enableBg[2].v1); ImGui::SameLine();
        ImGui::Checkbox("BG3", &board.ppu.debug.enableBg[3].v1);
        ImGui::Checkbox("SP ", &board.ppu.debug.enableSp.v1);
        ImGui::Separator();
        ImGui::Text("Audio\n");
        ImGui::Checkbox("Ch0", &board.apu.debug.enableCh[0 ].v1); ImGui::SameLine();
        ImGui::Checkbox("Ch1", &board.apu.debug.enableCh[1 ].v1); ImGui::SameLine();
        ImGui::Checkbox("Ch2", &board.apu.debug.enableCh[2 ].v1); ImGui::SameLine();
        ImGui::Checkbox("Ch3", &board.apu.debug.enableCh[3 ].v1);
        ImGui::Checkbox("Ch4", &board.apu.debug.enableCh[4 ].v1); ImGui::SameLine();
        ImGui::Checkbox("Ch5", &board.apu.debug.enableCh[5 ].v1); ImGui::SameLine();
        ImGui::Checkbox("Ch6", &board.apu.debug.enableCh[6 ].v1); ImGui::SameLine();
        ImGui::Checkbox("Ch7", &board.apu.debug.enableCh[7 ].v1);
        ImGui::Separator();
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
      ImGui::EndChild();
      ImGui::SameLine();
      ImGui::BeginChild(ImGui::GetID((void*)3), ImVec2(wC, hLC), ImGuiWindowFlags_NoTitleBar);
      {
        float gameW = wC;
        float gameH = (float)HW_SCREEN_H / (float)HW_SCREEN_W * gameW;
        ImGui::Image(reinterpret_cast<void *>(framebuffer), ImVec2(gameW, gameH));
      }
      ImGui::EndChild();
    ImGui::EndChild();
    ImGui::BeginChild(ImGui::GetID((void*)4), ImVec2(wB, hB), ImGuiWindowFlags_NoTitleBar);
    {
      ImGui::Text("Viewer Panel");
      // for (auto& s: disasms) {
      //   ImGui::Text("%s", s.c_str());
      // }
    }
    ImGui::EndChild();
  ImGui::EndChild();
  ImGui::SameLine();
  ImGui::BeginChild(ImGui::GetID((void*)5), ImVec2(wR, hR), ImGuiWindowFlags_NoTitleBar);
  {
    ImGui::Text("Property Panel\nHwREG\n");
  }
  ImGui::EndChild();
  ImGui::PopStyleVar(3);
}

void MainComponent::renderFramebuffer()
{
  int width = HW_SCREEN_W, height = HW_SCREEN_H;
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glViewport(0, 0, HW_SCREEN_W, HW_SCREEN_H);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
}

void MainComponent::cleanup()
{
  glDeleteRenderbuffers(1, &renderbuffer);
  glDeleteTextures(1, &texture);
  glDeleteFramebuffers(1, &framebuffer);
}

void DrawSplitter(int split_vertically, float thickness, float* size0, float* size1, float min_size0, float min_size1)
{
  ImVec2 backup_pos = ImGui::GetCursorPos();
  if (split_vertically)
      ImGui::SetCursorPosY(backup_pos.y + *size0);
  else
      ImGui::SetCursorPosX(backup_pos.x + *size0);

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
  // We don't draw while active/pressed because as we move the panes the splitter button will be 1 frame late
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0,0,0,0));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f,0.6f,0.6f,0.10f));
  ImGui::Button("##Splitter", ImVec2(!split_vertically ? thickness : -1.0f, split_vertically ? thickness : -1.0f));
  ImGui::PopStyleColor(3);

  ImGui::SetItemAllowOverlap(); // This is to allow having other buttons OVER our splitter. 

  if (ImGui::IsItemActive())
  {
    float mouse_delta = split_vertically ? ImGui::GetIO().MouseDelta.y : ImGui::GetIO().MouseDelta.x;

    // Minimum pane size
    if (mouse_delta < min_size0 - *size0)
        mouse_delta = min_size0 - *size0;
    if (mouse_delta > *size1 - min_size1)
        mouse_delta = *size1 - min_size1;

    // Apply resize
    *size0 += mouse_delta;
    *size1 -= mouse_delta;
  }
  ImGui::SetCursorPos(backup_pos);
}
