#pragma once
#include <common.h>
#include <memorymap.h>
#include <memory.h>
#include <string>
#include <vector>

using namespace std;

class IntrrCtrl;

class Sram : public MemorySection {
private:
  Memory& memory;
  IntrrCtrl& intrrCtrl;
  string path;
public:
  Sram(Memory& memory, IntrrCtrl& intrrCtrl);
  ~Sram();
  auto init() -> void;
  auto load(const string& path) -> uint8_t;
  auto save(const string& path) -> uint8_t;
  auto read8(uint32_t addr) -> int8_t override;
  auto read16(uint32_t addr) -> int16_t override;
  auto read32(uint32_t addr) -> int32_t override;
  auto read(uint32_t addr, uint32_t size) -> int32_t override;
  auto write8(uint32_t addr, int8_t value) -> void override;
  auto write16(uint32_t addr, int16_t value) -> void override;
  auto write32(uint32_t addr, int32_t value) -> void override;
  auto write(uint32_t addr, uint32_t size, int32_t value) -> void override;
};
