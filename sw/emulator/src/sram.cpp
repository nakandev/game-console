#include <sram.h>
#include <memory.h>
#include <intrrctrl.h>
#include <fmt/core.h>

Sram::Sram(Memory& memory, IntrrCtrl& intrrCtrl)
: MemorySection("save", HWREG_SAVERAM_BASEADDR, HWREG_SAVERAM_SIZE),
  memory(memory), intrrCtrl(intrrCtrl)
{
  init();
}

Sram::~Sram()
{
}

auto Sram::init() -> void
{
  memory.addSection<Sram>(this);
}

auto Sram::load(const string& path) -> uint8_t
{
  return 0;
}

auto Sram::save(const string& path) -> uint8_t
{
  return 0;
}

auto Sram::read8(uint32_t addr) -> int8_t
{
  return MemorySection::read8(addr);
}

auto Sram::read16(uint32_t addr) -> int16_t
{
  return MemorySection::read16(addr);
}

auto Sram::read32(uint32_t addr) -> int32_t
{
  return MemorySection::read32(addr);
}

auto Sram::read(uint32_t addr, uint32_t size) -> int32_t
{
  return MemorySection::read(addr, size);
}

auto Sram::write8(uint32_t addr, int8_t value) -> void
{
  MemorySection::write8(addr, value);
}

auto Sram::write16(uint32_t addr, int16_t value) -> void
{
  MemorySection::write16(addr, value);
}

auto Sram::write32(uint32_t addr, int32_t value) -> void
{
  MemorySection::write32(addr, value);
}

auto Sram::write(uint32_t addr, uint32_t size, int32_t value) -> void
{
  MemorySection::write(addr, size, value);
}

