#include <dma.h>
#include <memory.h>
#include <fmt/core.h>

Dma::Dma(Memory& memory)
  : memory(memory)
{
  ioram = (HwIoRam*)memory.section("ioram").buffer();
  channels.resize(4);
  init();
}

Dma::~Dma()
{
  channels.clear();
}

void Dma::init()
{
  this->ioram = (HwIoRam*)memory.section("ioram").buffer();
  HwIoRam& ioram = *this->ioram;
  for (auto& ch: channels) {
    ch.src = 0;
    ch.dst = 0;
    ch.attribute = 0;
    ch.data = 0;
  }
  for (auto& dma: ioram.dma.dma) {
    dma.src = 0;
    dma.dst = 0;
    dma.attribute = 0;
  }
}

void Dma::syncFromIoDma(int chIdx)
{
  HwIoRam& ioram = *this->ioram;
  channels[chIdx].src          = ioram.dma.dma[chIdx].src         ;
  channels[chIdx].dst          = ioram.dma.dma[chIdx].dst         ;
  channels[chIdx].attribute    = ioram.dma.dma[chIdx].attribute   ;
  channels[chIdx].isWrite = false;
  channels[chIdx].isRunning = false;
}

void Dma::stepCpuCycle()
{
  HwIoRam& ioram = *this->ioram;
  int bytesizeTable[4] = {1, 2, 4, 4};
  for (int chIdx = 0; chIdx<channels.size(); chIdx++) {
    auto& ch = channels[chIdx];
    uint32_t priority = chIdx + 1;
    if (memory.isBusy(priority + 1)) {
      continue;
    }
    uint8_t bytesize = bytesizeTable[ch.size];
    if (!ch.isRunning) {
      syncFromIoDma(chIdx);
      if (ch.enable) {
        memory.setBusy(priority);
        channels[chIdx].isRunning = true;
      }
    }
    if (ch.enable && ch.count > 0) {
      if (!ch.isWrite) {
        ch.data = memory.read(ch.src, bytesize);
      } else {
        memory.write(ch.dst, bytesize, ch.data);
        ch.count--;
        switch (ch.srcIncrement) {
          case 0: ch.src += bytesize; break;
          case 1: ch.src -= bytesize; break;
          // case 2: break;
          default: break;
        }
        switch (ch.dstIncrement) {
          case 0: ch.dst += bytesize; break;
          case 1: ch.dst -= bytesize; break;
          // case 2: break;
          default: break;
        }
      }
      ch.isWrite = ~ch.isWrite;
    } else
    if (ch.enable && ch.count == 0) {
      ch.enable = false;
      ch.isRunning = false;
      memory.clearBusy(priority);
      ioram.dma.dma[chIdx].enable = false;
      ioram.dma.dma[chIdx].count = 0;
      // occur interruption
    }
  }
}


void Memory::dmatransfer(uint32_t src, uint32_t dst, uint8_t direction, uint8_t len, uint32_t count)
{
  auto& srcSection = sectionByAddr(src);
  auto& dstSection = sectionByAddr(dst);
  uint8_t* const srcBuffer = srcSection.buffer();
  uint8_t* const dstBuffer = dstSection.buffer();
  // bit[5:3]=dst {0:inc, 1:dec, 2:fix}, bit[2:0]=src {0:inc, 1:dec, 2:fix}
  if (direction == 0) {
    uint8_t* srcptr = srcBuffer;
    uint8_t* dstptr = dstBuffer;
    for (int i=0; i<len * count; i++) {
      *dstptr = *srcptr;
      dstptr++;
      srcptr++;
    }
  } else
  if (direction == 1) {
    uint8_t* srcptr = srcBuffer;
    uint8_t* dstptr = dstBuffer + len * count - 1;
    for (int i=0; i<len * count; i++) {
      *dstptr = *srcptr;
      dstptr--;
      srcptr++;
    }
  } else
  if (direction == 2) {
    uint8_t* srcptr = srcBuffer;
    uint8_t* dstptr = dstBuffer;
    for (int i=0; i<len * count; i++) {
      *dstptr = *srcptr;
      srcptr++;
    }
  } else
  if (direction == 4) {
    uint8_t* srcptr = srcBuffer + len * count - 1;
    uint8_t* dstptr = dstBuffer;
    for (int i=0; i<len * count; i++) {
      *dstptr = *srcptr;
      dstptr++;
      srcptr--;
    }
  } else
  if (direction == 5) {
    uint8_t* srcptr = srcBuffer + len * count - 1;
    uint8_t* dstptr = dstBuffer + len * count - 1;
    for (int i=0; i<len * count; i++) {
      *dstptr = *srcptr;
      dstptr--;
      srcptr--;
    }
  } else
  if (direction == 6) {
    uint8_t* srcptr = srcBuffer + len * count - 1;
    uint8_t* dstptr = dstBuffer;
    for (int i=0; i<len * count; i++) {
      *dstptr = *srcptr;
      srcptr--;
    }
  } else
  if (direction == 8) {
    uint8_t* srcptr = srcBuffer;
    uint8_t* dstptr = dstBuffer;
    for (int i=0; i<len * count; i++) {
      *dstptr = *srcptr;
      dstptr++;
      srcptr;
    }
  } else
  if (direction == 9) {
    uint8_t* srcptr = srcBuffer;
    uint8_t* dstptr = dstBuffer + len * count - 1;
    for (int i=0; i<len * count; i++) {
      *dstptr = *srcptr;
      dstptr--;
      srcptr;
    }
  } else
  if (direction == 10) {
    uint8_t* srcptr = srcBuffer;
    uint8_t* dstptr = dstBuffer;
    for (int i=0; i<len * count; i++) {
      *dstptr = *srcptr;
    }
  } else
  {
  }
}

void Memory::checkDma()
{
}
