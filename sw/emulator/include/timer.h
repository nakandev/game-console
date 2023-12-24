#pragma once
#include <common.h>
#include <memorymap.h>
#include <vector>

using namespace std;

class Memory;
class IntrrCtrl;

class Timer {
private:
  Memory& memory;
  IntrrCtrl& intrrCtrl;
  HwIoRam* ioram;
public:
  Timer(Memory& memory, IntrrCtrl& intrrCtrl);
  ~Timer();
  auto init() -> void;
  auto stepCycle() -> void;
};

