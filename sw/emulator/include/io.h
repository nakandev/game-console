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
  void setCpu(Cpu& cpu);
  void setVpu(Vpu& vpu);
  void setApu(Apu& apu);
  void setDma(Dma& dma);
  void setTimer(Timer& timer);
  void pressPadButton(uint8_t button);
  void releasePadButton(uint8_t button);
  HwPad getPadStatus();
  void requestInt(uint8_t intno);
  void setIntStatus(uint8_t intno);
  void clearIntStatus(uint8_t intno);
  void updateScanlineNumber(uint16_t y);
};
