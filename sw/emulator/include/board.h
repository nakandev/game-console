#pragma once
#include <memorymap.h>
#include <memory.h>
#include <cpu.h>
#include <ppu.h>
#include <apu.h>
#include <io.h>

class Board {
public:
  uint64_t masterClock;
  Memory memory;
  IO io;
  Cpu cpu;
  Ppu ppu;
  Apu apu;
  bool pause;
  Board();
  ~Board();
  void initMemory();
  void reset();
  void updateFrameUntilVblank();
  void updateFrameSinceVblank();
};
