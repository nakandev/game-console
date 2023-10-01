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
  void requestExtInt(uint8_t intno);
  void setExtIntStatus(uint8_t intno);
  void clearExtIntStatus(uint8_t intno);
};
