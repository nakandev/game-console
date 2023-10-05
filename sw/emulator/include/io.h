#pragma once
#include <cstdint>
#include <memorymap.h>
class Memory;
class Cpu;
class Ppu;
class Apu;

class IO {
private:
  Memory& memory;
  Cpu* cpu;
  Ppu* ppu;
  Apu* apu;
public:
  IO(Memory& memory);
  ~IO();
  void setCpu(Cpu& cpu);
  void setPpu(Ppu& ppu);
  void setApu(Apu& apu);
  void pressPadButton(uint8_t button);
  void releasePadButton(uint8_t button);
  HwPad getPadStatus();
  void requestInt(uint8_t intno);
  void setIntStatus(uint8_t intno);
  void clearIntStatus(uint8_t intno);
  void updateScanlineNumber(uint16_t y);
};
