#pragma once
#include <common.h>
#include <memorymap.h>
#include <vector>

using namespace std;

class Memory;
class IO;

class Timer {
private:
  Memory& memory;
  IO& io;
  HwIoRam* ioram;
public:
  Timer(Memory& memory, IO& io);
  ~Timer();
  void init();
  void stepCpuCycle();
};
