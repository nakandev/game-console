#pragma once
#include <common.h>
#include <memorymap.h>
#include <memory.h>
#include <vector>

using namespace std;

class IntrrCtrl;

struct TimerChannel {
  uint32_t time;
  uint32_t count;
};

class Timer : public MemorySection {
private:
  Memory& memory;
  IntrrCtrl& intrrCtrl;
  vector<TimerChannel> channels;
public:
  Timer(Memory& memory, IntrrCtrl& intrrCtrl);
  ~Timer();
  auto init() -> void;
  auto stepCycle() -> void;
  auto write8(uint32_t addr, int8_t value) -> void override;
  auto write16(uint32_t addr, int16_t value) -> void override;
  auto write32(uint32_t addr, int32_t value) -> void override;
  auto write(uint32_t addr, uint32_t size, int32_t value) -> void override;
};

