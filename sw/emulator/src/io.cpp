#include <io.h>
#include <memory.h>
#include <cpu.h>
#include <ppu.h>
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
void IO::setPpu(Ppu& ppu)
{
  this->ppu = &ppu;
}
void IO::setApu(Apu& apu)
{
  this->apu = &apu;
}

void IO::pressPadButton(uint8_t button)
{
  const uint32_t addr = HW_IO_PAD_ADDR;
  uint32_t status = memory.read(addr, 4);
  status |= 1u << button;
  memory.write(addr, 4, status);
}

void IO::releasePadButton(uint8_t button)
{
  const uint32_t addr = HW_IO_PAD_ADDR;
  uint32_t status = memory.read(addr, 4);
  status &= ~(1u << button);
  memory.write(addr, 4, status);
}

void IO::requestExtInt(uint8_t intno)
{
  const uint32_t addr = HW_IO_EXTINT_ENABLE_ADDR;
  uint32_t status = memory.read(addr, 4);
  if (status & (1u << intno)) {
    cpu->requestInterruption();
  }
  clearExtIntStatus(intno);
}

void IO::setExtIntStatus(uint8_t intno)
{
  const uint32_t addr = HW_IO_EXTINT_STATUS_ADDR;
  uint32_t status = memory.read(addr, 4);
  status |= 1u << intno;
  memory.write(addr, 4, status);
}

void IO::clearExtIntStatus(uint8_t intno)
{
  const uint32_t addr = HW_IO_EXTINT_STATUS_ADDR;
  uint32_t status = memory.read(addr, 4);
  status &= ~(1u << intno);
  memory.write(addr, 4, status);
}
