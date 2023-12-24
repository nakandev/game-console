#include <dma.h>
#include <memory.h>
#include <intrrctrl.h>
#include <fmt/core.h>

Dma::Dma(Memory& memory, IntrrCtrl& intrrCtrl)
: Processor(), MemorySection("dma", HWREG_IO_DMA_ADDR, 0x100),
  memory(memory), intrrCtrl(intrrCtrl), channels(), runningDma(-1)
{
  channels.resize(4);
  init();
}

Dma::~Dma()
{
  channels.clear();
}

auto Dma::init() -> void
{
  memory.addSection<Dma>(*this);
  HwIoDma& dmaram = *(HwIoDma*)this->data;
  for (auto& ch: channels) {
    ch.src = 0;
    ch.dst = 0;
    ch.attribute = 0;
    ch.data = 0;
    ch.isWrite = false;
    ch.isRunning = false;
  }
  for (auto& dma: dmaram.dma) {
    dma.src = 0;
    dma.dst = 0;
    dma.attribute = 0;
  }
  runningDma = -1;
}

auto Dma::syncFromIoDma(int chIdx) -> void
{
  HwIoDma& dma = *(HwIoDma*)this->data;
  channels[chIdx].src          = dma.dma[chIdx].src         ;
  channels[chIdx].dst          = dma.dma[chIdx].dst         ;
  channels[chIdx].attribute    = dma.dma[chIdx].attribute   ;
  channels[chIdx].isWrite = false;
  channels[chIdx].isRunning = true;
  channels[chIdx].enable = true;
}

auto Dma::stepCycle() -> void
{
  HwIoDma& dma = *(HwIoDma*)this->data;
  int chIdx = runningDma;
  auto& ch = channels[chIdx];
  uint32_t priority = chIdx + 1;
  if (dma.dma[chIdx].enable && !ch.isRunning) {
    syncFromIoDma(chIdx);
    memory.setBusy(priority);
  }
  int bytesizeTable[4] = {1, 2, 4, 4};
  uint8_t bytesize = bytesizeTable[ch.size];
  if (ch.count > 0) {
    if (!ch.isWrite) {
      switch (ch.size) {
        case HW_IO_DMA_SIZE_8BIT:  ch.data = memory.read8(ch.src); break;
        case HW_IO_DMA_SIZE_16BIT: ch.data = memory.read16(ch.src); break;
        default: ch.data = memory.read32(ch.src); break;
      }
      // ch.data = memory.read(ch.src, bytesize);
    } else {
      switch (ch.size) {
        case HW_IO_DMA_SIZE_8BIT:  memory.write8(ch.dst, ch.data); break;
        case HW_IO_DMA_SIZE_16BIT: memory.write16(ch.dst, ch.data); break;
        default: memory.write32(ch.dst, ch.data); break;
      }
      // memory.write(ch.dst, bytesize, ch.data);
      ch.count--;
      switch (ch.srcIncrement) {
        case HW_IO_DMA_INCREMENT_INC: ch.src += bytesize; break;
        case HW_IO_DMA_INCREMENT_DEC: ch.src -= bytesize; break;
        // case HW_IO_DMA_INCREMENT_STAY: break;
        default: break;
      }
      switch (ch.dstIncrement) {
        case HW_IO_DMA_INCREMENT_INC: ch.dst += bytesize; break;
        case HW_IO_DMA_INCREMENT_DEC: ch.dst -= bytesize; break;
        // case HW_IO_DMA_INCREMENT_STAY: break;
        default: break;
      }
    }
    ch.isWrite = ~ch.isWrite;
  } else
  if (ch.count == 0) {
    ch.enable = false;
    ch.isRunning = false;
    dma.dma[chIdx].enable = false;
    dma.dma[chIdx].count = 0;
    runningDma = -1;
    for (int i=3; i>=0; i--) {
      auto& ch = dma.dma[i];
      if (ch.enable) {
        runningDma = i;
        break;
      }
    }
    if (runningDma == -1) {
      memory.processor = this;
    }
    memory.clearBusy(priority);
    memory.processor = nullptr;
    if (dma.dma[chIdx].interrupt) {
      if (dma.dma[chIdx].trigger == HW_IO_DMA_TRIGGER_IMMEDIATELY) {
        uint32_t intno = HW_IO_INT_DMA0 + chIdx;
        intrrCtrl.setIntStatus(intno);
        intrrCtrl.requestInt(intno);
      }
    }
  }
}

auto Dma::write8(uint32_t addr, int8_t value) -> void
{
  MemorySection::write8(addr, value);
  updateRunningDma(addr, value);
}
auto Dma::write16(uint32_t addr, int16_t value) -> void
{
  MemorySection::write16(addr, value);
  updateRunningDma(addr, value);
}
auto Dma::write32(uint32_t addr, int32_t value) -> void
{
  MemorySection::write32(addr, value);
  updateRunningDma(addr, value);
}

auto Dma::write(uint32_t addr, uint32_t size, int32_t value) -> void
{
  MemorySection::write(addr, size, value);
  updateRunningDma(addr, value);
}

auto Dma::updateRunningDma(uint32_t addr, int32_t value) -> void
{
  HwIoDma& dma = *(HwIoDma*)this->data;
  runningDma = -1;
  if (dma.dma[0].enable) runningDma = 0;
  if (dma.dma[1].enable) runningDma = 1;
  if (dma.dma[2].enable) runningDma = 2;
  if (dma.dma[3].enable) runningDma = 3;
  if (runningDma != -1) {
    memory.processor = this;
  }
  fmt::print("runningDma={}\n", runningDma);
}
