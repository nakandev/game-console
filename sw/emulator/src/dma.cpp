#include <dma.h>
#include <memory.h>
#include <io.h>
#include <fmt/core.h>

Dma::Dma(Memory& memory, IO& io)
  : memory(memory), io(io), ioram(), channels(), runningDma(-1)
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
    ch.isWrite = false;
    ch.isRunning = false;
  }
  for (auto& dma: ioram.dma.dma) {
    dma.src = 0;
    dma.dst = 0;
    dma.attribute = 0;
  }
  runningDma = -1;
}

void Dma::syncFromIoDma(int chIdx)
{
  HwIoRam& ioram = *this->ioram;
  channels[chIdx].src          = ioram.dma.dma[chIdx].src         ;
  channels[chIdx].dst          = ioram.dma.dma[chIdx].dst         ;
  channels[chIdx].attribute    = ioram.dma.dma[chIdx].attribute   ;
  channels[chIdx].isWrite = false;
  channels[chIdx].isRunning = true;
  channels[chIdx].enable = true;
}

void Dma::stepCpuCycle()
{
  if (!isRunning()) {
    return;
  }
  HwIoRam& ioram = *this->ioram;
  int chIdx = runningDma;
  auto& ch = channels[chIdx];
  uint32_t priority = chIdx + 1;
  if (ioram.dma.dma[chIdx].enable && !ch.isRunning) {
    syncFromIoDma(chIdx);
    memory.setBusy(priority);
  }
  int bytesizeTable[4] = {1, 2, 4, 4};
  uint8_t bytesize = bytesizeTable[ch.size];
  if (ch.count > 0) {
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
  if (ch.count == 0) {
    ch.enable = false;
    ch.isRunning = false;
    ioram.dma.dma[chIdx].enable = false;
    ioram.dma.dma[chIdx].count = 0;
    runningDma = -1;
    memory.clearBusy(priority);
    uint32_t intno = HW_IO_INT_DMA0 + chIdx;
    io.setIntStatus(intno);
    io.requestInt(intno);
  }
}

bool Dma::isRunning()
{
  HwIoRam& ioram = *this->ioram;
  for (int i=3; i>=0; i--) {
    auto& ch = ioram.dma.dma[i];
    if (ch.enable) {
      runningDma = i;
      return true;
    }
  }
  runningDma = -1;
  return false;
};

void Memory::checkDma()
{
}
