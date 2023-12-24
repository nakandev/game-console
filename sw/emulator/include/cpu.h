#pragma once
#include <memory>
#include <register.h>
#include <processor.h>
#include <cpuisa.h>
#include <memory.h>
#include <elf.h>
#include <unordered_map>

class Cpu : public Processor {
private:
  MemorySection* programSection;
  shared_ptr<Elf> elf;
  CpuIsaRV32I isa;
  vector<Instruction> currentInstr;
  int64_t maxCycles;
  int64_t cycleCount;
  int64_t instrCount;
  RegisterSet regs;
  Memory& memory;
  unordered_map<uint32_t, Instruction> instrCache;
protected:
  void setEntrypoint(uint32_t entry);
public:
  Cpu(Memory& memory);
  ~Cpu();
  auto setMaxCycles(int64_t cycles) -> void;
  auto loadElf(const string& path) -> uint8_t;
  auto init() -> void;
  auto reset() -> void;
  auto requestInterruption() -> void;
  auto handleInterruption() -> void;
  auto stepCycle() -> void override;
  auto stepInstruction() -> void;
  const auto getPc() -> uint32_t;
  auto cacheAllInstruction() -> void;
  const auto disassembleAll() -> map<uint32_t, string>;
  const auto readRegisterAll() -> vector<string>;
};

