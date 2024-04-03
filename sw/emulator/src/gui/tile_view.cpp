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

const int tileBankNum = 8;
const int tileBankSize = 1024;

TileView::TileView(Board& board)
: board(board)
{
  bankNo = 0;
  palNo = 1;
  txn = 64; tyn = tileBankSize / txn;
  tiles.resize(HW_TILE_W * HW_TILE_H * tileBankSize);
  createFramebuffer();
}

TileView::~TileView()
{
}

auto TileView::update() -> void
{
  HwTileRam& tileram = *(HwTileRam*)board.memory.section("tile").buffer();
  int width = HW_TILE_W * txn, height = HW_TILE_H * tyn;
  for (int i=0; i<tileBankSize; i++) {
    for (int y=0; y<HW_TILE_H; y++) {
      for (int x=0; x<HW_TILE_W; x++) {
        // [TODO] select 256-color / 16-color
        uint8_t tidx = tileram.tile[bankNo][i].data[y][x];
        uint32_t color = tidx * 0x111111u | 0xFF000000u;
        int offset = x + y*HW_TILE_W*txn + i%txn*HW_TILE_W + i/txn*HW_TILE_H*txn*HW_TILE_W;
        tiles[offset] = color;
      }
    }
  }
  // ImVec2 backup_pos = ImGui::GetCursorPos();
  if(ImGui::InputInt("tileBank", &bankNo)) {
    if (bankNo < 0) bankNo = 0;
    if (bankNo > tileBankNum - 1) bankNo = tileBankNum - 1;
  }
  ImVec2 backup_pos = ImGui::GetCursorPos();
  ImGui::Image(reinterpret_cast<void *>(framebuffer), ImVec2(width, height));
}

auto TileView::createFramebuffer() -> void
{
  int width = HW_TILE_W * txn, height = HW_TILE_H * tyn;
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

auto TileView::renderFramebuffer() -> void
{
  HwTileRam& tileram = *(HwTileRam*)board.memory.section("tile").buffer();
  int width = HW_TILE_W * txn, height = HW_TILE_H * tyn;
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glViewport(0, 0, width, height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tiles.data());
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tileram.tile[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
}

auto TileView::cleanup() -> void
{
  glDeleteRenderbuffers(1, &renderbuffer);
  glDeleteTextures(1, &texture);
  glDeleteFramebuffers(1, &framebuffer);
}
