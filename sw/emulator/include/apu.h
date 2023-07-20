#pragma once
#include <common.h>
#include <memorymap.h>
#include <vector>

using namespace std;

class Memory;

struct ApuMusicData {
  uint32_t noteTime;  // auto update
  uint32_t frameTime;  // auto update
  int16_t* buffer;
};

class Apu {
private:
  Memory& memory;
  vector<int16_t> musicBuffer;
public:
  ApuMusicData apuMusicData;
  Apu(Memory& memory);
  ~Apu();
  void updateMusicBuffer();
};
