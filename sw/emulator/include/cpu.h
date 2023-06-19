#pragma once
#include <memory>
#include <register.h>
#include <instruction.h>
#include <memory.h>
#include <elf.h>

class Cpu {
private:
  vector<uint8_t> programCode;
  shared_ptr<Elf> elf;
  InstrRV32IManipulator instrManip;
  vector<Instruction> currentInstr;
  int64_t maxCycles;
  int64_t cycleCount;
  int64_t instrCount;
  RegisterSet regs;
  Memory& memory;
public:

  Cpu(Memory& memory);
  ~Cpu();
  void setMaxCycles(int64_t cycles);
  void loadBinary(vector<uint8_t> binary);
  void loadElf(const string& path);
  void initRegs();
  void setEntrypoint(uint32_t entry);
  void launch(uint32_t entry=0);
  void requestInterruption();
  void stepi();
  void printPc();
};

