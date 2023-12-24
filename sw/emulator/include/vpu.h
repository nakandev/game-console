#pragma once
#include <common.h>
#include <memorymap.h>
#include <vector>

using namespace std;

class Memory;

struct VpuSprite {
  HwSprite hwsp;
  int16_t beginX;
  int16_t endX;
  int16_t beginY;
  int16_t endY;
  uint8_t idx;
};

class Vpu {
private:
  int _currentLineNo;
  vector<vector<uint32_t>> lineBufferBg;
  vector<vector<uint32_t>> lineBufferSp;
  vector<uint32_t> screenBuffer;
  vector<VpuSprite> vpuSprite;
  Memory& memory;
  int scanline;
  auto setHBlank(bool flag) -> void;
  auto setVBlank(bool flag) -> void;
public:
  struct {
    vector<bool8_t> enableBg;
    bool8_t enableSp;
  } debug;
  Vpu(Memory& memory);
  ~Vpu();
  auto init() -> void;
  auto currentLineNo() -> int;
  auto drawLine(int y) -> void;
  auto drawAllLine() -> void;
  auto copyScreenBuffer(uint32_t* buffer, bool inv=false) -> void;
};
