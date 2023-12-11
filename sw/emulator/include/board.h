#pragma once
#include <memorymap.h>
#include <memory.h>
#include <cpu.h>
#include <vpu.h>
#include <apu.h>
#include <io.h>
#include <dma.h>
#include <timer.h>
#include <string>

class Board {
public:
  uint64_t masterClock;
  Memory memory;
  IO io;
  Cpu cpu;
  Vpu vpu;
  Apu apu;
  Dma dma;
  Timer timer;
  bool pause;
  Board();
  ~Board();
  void initMemory();
  void reset();
  void updateFrameUntilVblank();
  void updateFrameSinceVblank();
  void loadElf(const string& path);
  void reloadElf();
  void saveState(const string& path);
  void loadState(const string& path);
};
