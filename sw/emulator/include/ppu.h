#pragma once
#include <common.h>
#include <vector>

using namespace std;

class Memory;

class Ppu {
private:
  int _currentLineNo;
  vector<uint32_t> lineBufferBg0;
  vector<uint32_t> lineBufferBg1;
  vector<uint32_t> lineBufferBg2;
  vector<uint32_t> lineBufferBg3;
  vector<uint32_t> lineBufferSp;
  vector<uint32_t> screenBuffer;
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
  void printTile();
};
