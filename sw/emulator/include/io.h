#pragma once
#include <cstdint>
#include <memorymap.h>
class Memory;
class Cpu;
class Vpu;
class Apu;
class Dma;
class Timer;

class IO {
private:
  Memory& memory;
  Cpu* cpu;
  Vpu* vpu;
  Apu* apu;
  Dma* dma;
  Timer* timer;
public:
  IO(Memory& memory);
  ~IO();
  auto setCpu(Cpu& cpu) -> void;
  auto setVpu(Vpu& vpu) -> void;
  auto setApu(Apu& apu) -> void;
  auto setDma(Dma& dma) -> void;
  auto setTimer(Timer& timer) -> void;
  auto pressPadButton(uint8_t button) -> void;
  auto releasePadButton(uint8_t button) -> void;
  auto getPadStatus() -> HwPad;
  auto updateScanlineNumber(uint16_t y) -> void;
};
