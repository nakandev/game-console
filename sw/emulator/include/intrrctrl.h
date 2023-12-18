#pragma once
#include <common.h>
#include <memorymap.h>

class Memory;
class Cpu;

class IntrrCtrl {
public:
  Memory& memory;
  Cpu& cpu;
  IntrrCtrl(Memory& memory, Cpu& cpu);
  ~IntrrCtrl();
  void requestInt(uint8_t intno);
  void setIntStatus(uint8_t intno);
  void clearIntStatus(uint8_t intno);
};

