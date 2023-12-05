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
  guiDebug(false),
  debugPanel(board),
  disasmView(board),
  paletteView(board),
  tileView(board),
  mmioPanel(board),
  ctrlTabIndex(),
  viewTabIndex(),
  propertyTabIndex()
{
  screenBuffer.resize(HW_SCREEN_W * HW_SCREEN_H);
  screenScale = 1;
  scaleMode = SCREEN_SCALE_FIT;
  hMenu = 20;
  margin = 8;
  thick = 10;
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  wSCR = io.DisplaySize.x - margin*2, hSCR = io.DisplaySize.y - margin*2 - hMenu;
  wL = 220;
  wR = 220;
  wC = wSCR-(wL+wR)-thick;
  wLC = wL + wC;
  hB = 220;
  hR = hSCR;
  hC = hSCR-hB-thick;
  hLC = hC;
}

MainComponent::~MainComponent()
{
}
void MainComponent::setElfPath(string path)
{
  elfpath = path;
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
        int ret = board.cpu.loadElf(elfpath);
        if (ret) {
          fmt::print("cannot open ELF: {}\n", (string)elfpath);
        }
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
      board.reset();
      if(!board.cpu.loadElf(elfpath)) {
        board.pause = false;
        disasmView.disasmStrs = board.cpu.disassembleAll();
      }
    }
    ImGuiFileDialog::Instance()->Close();
  }
}

void MainComponent::renderMainPanel()
{
  ImVec2 winsize = ImGui::GetWindowSize();
  // fmt::print("winW={}, winH={}\n", (int)winsize.x, (int)winsize.y);
  wSCR = winsize.x - margin*2, hSCR = winsize.y - margin*2 - hMenu;
  if (guiDebug) {
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
          debugPanel.update();
        }
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild(ImGui::GetID((void*)3), ImVec2(wC, hLC), ImGuiWindowFlags_NoTitleBar);
        {
          renderScreen(wC, hC, false);
        }
        ImGui::EndChild();
      ImGui::EndChild();
      ImGui::BeginChild(ImGui::GetID((void*)4), ImVec2(wB, hB), ImGuiWindowFlags_NoTitleBar);
      {
        ImGui::Text("Viewer Panel");
        ImGui::RadioButton("Disassembly", &viewTabIndex, 0); ImGui::SameLine();
        ImGui::RadioButton("Palette"    , &viewTabIndex, 1); ImGui::SameLine();
        ImGui::RadioButton("Tile"       , &viewTabIndex, 2); ImGui::SameLine();
        ImGui::RadioButton("Tilemap"    , &viewTabIndex, 3); //ImGui::SameLine();
        if      (viewTabIndex == 0) disasmView.update();
        else if (viewTabIndex == 1) paletteView.update();
        else if (viewTabIndex == 2) tileView.update();
      }
      ImGui::EndChild();
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild(ImGui::GetID((void*)5), ImVec2(wR, hR), ImGuiWindowFlags_NoTitleBar);
    {
      ImGui::Text("Property Panel\n");
      mmioPanel.update();
    }
    ImGui::EndChild();
    ImGui::PopStyleVar(3);
  }
  else {
    // ImGui::BeginChild(ImGui::GetID((void*)3), ImVec2(wSCR, hSCR), ImGuiWindowFlags_NoTitleBar);
    {
      renderScreen(wSCR, hSCR, true);
    }
    // ImGui::EndChild();
  }
}

void MainComponent::renderScreen(int w, int h, bool center)
{
  float aspect = (float)w / (float)h;
  float gameW, gameH;
  if (scaleMode == SCREEN_SCALE_FIT) {
    float scale = 1.0;
    if (aspect > 4.0/3.0) {
      scale = (float)h / (float)HW_SCREEN_H;
    } else {
      scale = (float)w / (float)HW_SCREEN_W;
    }
    gameW = (float)HW_SCREEN_W * scale;
    gameH = (float)HW_SCREEN_H * scale;
  } else
  if (scaleMode == SCREEN_SCALE_FIT_STRETCH) {
    gameW = w;
    gameH = h;
  } else
  if (scaleMode == SCREEN_SCALE_FIT_INT) {
    int scale = 1;
    if (aspect > 4.0/3.0) {
      scale = (h / HW_SCREEN_H);
    } else {
      scale = (w / HW_SCREEN_W);
    }
    gameW = HW_SCREEN_W * scale;
    gameH = HW_SCREEN_H * scale;
  } else
  {
    gameW = HW_SCREEN_W * screenScale;
    gameH = HW_SCREEN_H * screenScale;
  }
  int imgX = (w - gameW) / 2;
  int imgY = 0;
  if (center) {
    imgX = (w - gameW) / 2 + margin;
    imgY = (h - gameH) / 2 + margin + hMenu;
  }
  ImGui::SetCursorPos(ImVec2(imgX, imgY));
  ImGui::Image(reinterpret_cast<void *>(framebuffer), ImVec2(gameW, gameH));
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
