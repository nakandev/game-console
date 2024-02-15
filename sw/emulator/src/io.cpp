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

auto IO::setCpu(Cpu& cpu) -> void
{
  this->cpu = &cpu;
}

auto IO::setVpu(Vpu& vpu) -> void
{
  this->vpu = &vpu;
}

auto IO::setApu(Apu& apu) -> void
{
  this->apu = &apu;
}

auto IO::setDma(Dma& dma) -> void
{
  this->dma = &dma;
}

auto IO::setTimer(Timer& timer) -> void
{
  this->timer = &timer;
}

auto IO::pressPadButton(uint8_t button) -> void
{
  const uint32_t addr = HWREG_IO_PAD0_ADDR;
  uint32_t status = memory.read32(addr);
  status |= 1u << button;
  memory.write32(addr, status);
}

auto IO::releasePadButton(uint8_t button) -> void
{
  const uint32_t addr = HWREG_IO_PAD0_ADDR;
  uint32_t status = memory.read32(addr);
  status &= ~(1u << button);
  memory.write32(addr, status);
}

auto IO::getPadStatus() -> HwPad
{
  const uint32_t addr = HWREG_IO_PAD0_ADDR;
  uint32_t status = memory.read32(addr);
  HwPad hwpad = {.val32=status};
  return hwpad;
}

auto IO::updateScanlineNumber(uint16_t y) -> void
{
  memory.write16(HWREG_IO_SCANLINE_ADDR, y);
}

auto IO::updateMusicFrameNumber() -> void
{
  uint32_t frame = memory.read32(HWREG_IO_MUSICFRAME_ADDR);
  memory.write32(HWREG_IO_MUSICFRAME_ADDR, frame + 1);
}
