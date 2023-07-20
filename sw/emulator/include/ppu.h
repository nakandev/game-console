#pragma once
#include <common.h>
#include <memorymap.h>
#include <vector>

using namespace std;

class Memory;

struct PpuSprite {
  HwSprite hwsp;
  int16_t beginX;
  int16_t endX;
  int16_t beginY;
  int16_t endY;
  uint8_t idx;
};

class Ppu {
private:
  int _currentLineNo;
  vector<vector<uint32_t>> lineBufferBg;
  vector<vector<uint32_t>> lineBufferSp;
  vector<uint32_t> screenBuffer;
  vector<PpuSprite> ppuSprite;
  Memory& memory;
  int scanline;
  void setHBlank(bool flag);
  void setVBlank(bool flag);
public:
  Ppu(Memory& memory);
  ~Ppu();
  int currentLineNo();
  void drawLine(int y);
  void drawAllLine();
  void copyScreenBuffer(uint32_t* buffer);
};
