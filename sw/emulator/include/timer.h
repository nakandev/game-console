#pragma once
#include <common.h>
#include <memorymap.h>
#include <vector>

using namespace std;

class Memory;

class Timer {
private:
  Memory& memory;
  HwIoRam* ioram;
public:
  Timer(Memory& memory);
  ~Timer();
  void init();
  void stepCpuCycle();
};
