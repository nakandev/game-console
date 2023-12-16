#pragma once
#include <memory>
#include <register.h>
#include <cpuisa.h>
#include <memory.h>
#include <elf.h>
#include <unordered_map>

class Cpu {
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
  void setMaxCycles(int64_t cycles);
  uint8_t loadElf(const string& path);
  void init();
  void reset();
  void requestInterruption();
  void handleInterruption();
  void stepCycle();
  void stepInstruction();
  const uint32_t getPc();
  void cacheAllInstruction();
  const vector<string> disassembleAll();
  const vector<string> readRegisterAll();
};

