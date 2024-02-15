#pragma once
#include <memorymap.h>
#include <memory.h>
#include <cpu.h>
#include <intrrctrl.h>
#include <vpu.h>
#include <apu.h>
#include <dma.h>
#include <timer.h>
#include <io.h>
#include <sram.h>
#include <string>

class Board {
public:
  uint64_t masterClock;
  Memory memory;
  Cpu cpu;
  IntrrCtrl intrrCtrl;
  IO io;
  Vpu vpu;
  Apu apu;
  Dma dma;
  Timer timer;
  Sram sram;
  bool pause;
  Board();
  ~Board();
  auto reset() -> void;
  auto loadElf(const string& path) -> uint8_t;
  auto reloadElf() -> void;
  auto stepCpuCycle() -> void;
  auto updateFrame() -> void;
  auto saveState(const string& path) -> void;
  auto loadState(const string& path) -> void;
};
