#pragma once
#include <common.h>
#include <memorymap.h>
#include <memory.h>
#include <vector>

using namespace std;

class IntrrCtrl;

struct DmaChannel {
  uint32_t src;
  uint32_t dst;
  union {
    struct {
      uint32_t size  : 2;
      uint32_t count : 14;
      uint32_t enable : 1;
      uint32_t repeat : 1;
      uint32_t interrupt : 1;
      uint32_t mode : 1;
      uint32_t srcIncrement : 2;
      uint32_t dstIncrement : 2;
      uint32_t trigger : 4;
      uint32_t isWrite : 1;  // used by dma
      uint32_t isRunning: 1;  // used by dma
      uint32_t _reserved : 2;
    };
    uint32_t attribute;
  };
  uint32_t data;
};

class Dma : public Processor, public MemorySection {
private:
  Memory& memory;
  IntrrCtrl& intrrCtrl;
  vector<DmaChannel> channels;
  int runningDma;
  void syncFromIoDma(int chIdx);
  void updateRunningDma(uint32_t addr, int32_t value);
public:
  Dma(Memory& memory, IntrrCtrl& intrrCtrl);
  ~Dma();
  void init();
  void stepCycle();
  bool isRunning();
  void write8(uint32_t addr, int8_t value) override;
  void write16(uint32_t addr, int16_t value) override;
  void write32(uint32_t addr, int32_t value) override;
  void write(uint32_t addr, uint32_t size, int32_t value) override;
};
