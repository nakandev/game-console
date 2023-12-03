#pragma once
#include <common.h>
#include <memorymap.h>
#include <vector>

using namespace std;

class Memory;

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

class Dma {
private:
  Memory& memory;
  HwIoRam* ioram;
  vector<DmaChannel> channels;
  void syncFromIoDma(int chIdx);
public:
  Dma(Memory& memory);
  ~Dma();
  void init();
  void stepCpuCycle();
};
