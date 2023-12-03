#pragma once
#include <memory>
#include <register.h>
#include <instruction.h>
#include <memory.h>
#include <elf.h>

class Cpu {
private:
  MemorySection* programSection;
  shared_ptr<Elf> elf;
  InstrRV32IManipulator instrManip;
  vector<Instruction> currentInstr;
  int64_t maxCycles;
  int64_t cycleCount;
  int64_t instrCount;
  RegisterSet regs;
  Memory& memory;
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
  void printPc();
  const vector<string> disassembleAll();
  const vector<string> readRegisterAll();
};

