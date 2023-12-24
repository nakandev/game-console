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
  auto requestInt(uint8_t intno) -> void;
  auto setIntStatus(uint8_t intno) -> void;
  auto clearIntStatus(uint8_t intno) -> void;
};

