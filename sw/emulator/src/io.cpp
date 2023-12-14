#include <io.h>
#include <memory.h>
#include <cpu.h>
#include <vpu.h>
#include <apu.h>
#include <fmt/core.h>

IO::IO(Memory& memory)
  : memory(memory)
{
}

IO::~IO()
{
}

void IO::setCpu(Cpu& cpu)
{
  this->cpu = &cpu;
}
void IO::setVpu(Vpu& vpu)
{
  this->vpu = &vpu;
}
void IO::setApu(Apu& apu)
{
  this->apu = &apu;
}
void IO::setDma(Dma& dma)
{
  this->dma = &dma;
}
void IO::setTimer(Timer& timer)
{
  this->timer = &timer;
}

void IO::pressPadButton(uint8_t button)
{
  const uint32_t addr = HWREG_IO_PAD0_ADDR;
  uint32_t status = memory.read(addr, 4);
  status |= 1u << button;
  memory.write(addr, 4, status);
}

void IO::releasePadButton(uint8_t button)
{
  const uint32_t addr = HWREG_IO_PAD0_ADDR;
  uint32_t status = memory.read(addr, 4);
  status &= ~(1u << button);
  memory.write(addr, 4, status);
}

HwPad IO::getPadStatus()
{
  const uint32_t addr = HWREG_IO_PAD0_ADDR;
  uint32_t status = memory.read(addr, 4);
  HwPad hwpad = {.val32=status};
  return hwpad;
}

void IO::requestInt(uint8_t intno)
{
  uint32_t enable = memory.read(HWREG_IO_INT_ENABLE_ADDR, 4);
  uint32_t status = memory.read(HWREG_IO_INT_STATUS_ADDR, 4);
  if ((enable & (1u << intno)) && (status & (1u << intno))) {
    cpu->requestInterruption();
    cpu->handleInterruption();
  }
}

void IO::setIntStatus(uint8_t intno)
{
  uint32_t status = memory.read(HWREG_IO_INT_STATUS_ADDR, 4);
  status |= (1u << intno);
  memory.write(HWREG_IO_INT_STATUS_ADDR, 4, status);
}

void IO::clearIntStatus(uint8_t intno)
{
  uint32_t status = memory.read(HWREG_IO_INT_STATUS_ADDR, 4);
  status &= ~(1u << intno);
  memory.write(HWREG_IO_INT_STATUS_ADDR, 4, status);
}

void IO::updateScanlineNumber(uint16_t y)
{
  memory.write(HWREG_IO_SCANLINE_ADDR, 2, y);
}
