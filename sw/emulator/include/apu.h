#pragma once
#include <common.h>
#include <memorymap.h>
#include <vector>

using namespace std;

class Memory;

struct ApuMusicData {
  uint32_t noteCount;  // auto update
  uint32_t frameCount;  // auto update
  int16_t* buffer;
};

struct ApuMusicNote {
  uint32_t noteIdx;
  HwMusicNote note;
};

class Apu {
private:
  Memory& memory;
  vector<ApuMusicNote> noteBuffer;
  vector<int16_t> musicBuffer;
public:
  ApuMusicData apuMusicData;
  struct {
    vector<bool8_t> enableCh;
  } debug;
  Apu(Memory& memory);
  ~Apu();
  auto init() -> void;
  auto updateMusicBuffer() -> void;
};
