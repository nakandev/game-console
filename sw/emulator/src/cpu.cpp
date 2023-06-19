#include <cpu.h>

#include <fmt/core.h>
#include <cassert>

int debugLevel = 0;

Cpu::Cpu(Memory& memory)
  : programCode(), elf(), instrManip(), currentInstr(),
    maxCycles(5000), cycleCount(), instrCount(),
    regs(),
    memory(memory)
{
  currentInstr.resize(1);
}

Cpu::~Cpu()
{
}

void Cpu::setMaxCycles(int64_t cycles)
{
  maxCycles = cycles;
}

void Cpu::loadBinary(vector<uint8_t> binary)
{
  programCode = binary;
  memory.copy(0x0800'0000, binary.size(), binary.data());
}

void Cpu::loadElf(const string& path)
{
  elf = shared_ptr<Elf>(new Elf());
  elf.get()->load(path);
  elf.get()->allocMemory(memory);
}

void Cpu::initRegs()
{
  auto& stack = memory.section("stack");
  regs.gpr[2].val.u = stack.addr + stack.size - 4;
  if(debugLevel>=1) fmt::print("sp={:08x}\n",regs.gpr[2].val.u);
}

void Cpu::requestInterruption()
{
  regs.csr[MSTATUS].val.u |= 0x8;  // MIE bit on
}

void Cpu::stepi()
{
  bool isIntrr = instrManip.checkInterruption(currentInstr[0], regs, memory);
  if (isIntrr)
  {
    instrManip.handleInterruption(currentInstr[0], regs);
  }

  if (currentInstr[0].isWaiting) {
  } else
  {
    union32_t val = {.s=memory.read(regs.pc.val.u, 4)};
    instrManip.decode(val.u, currentInstr[0]);
    instrManip.execute(currentInstr[0], regs, memory);
    if (debugLevel >= 1) {
      instrManip.printInstrInfo(currentInstr[0], regs, memory);
    }
    if (!currentInstr[0].isJumped) {
      regs.pc.val.u += currentInstr[0].size;
      currentInstr[0].isJumped = false;
    }
  }
  cycleCount++;
  instrCount++;
}

void Cpu::setEntrypoint(uint32_t entry)
{
  regs.pc.val.u = entry;
}

void Cpu::launch(uint32_t entry)
{
  auto& stack = memory.section("stack");
  regs.gpr[2].val.u = stack.addr + stack.size - 4;
  if(debugLevel>=1) fmt::print("sp={:08x}\n",regs.gpr[2].val.u);
  setEntrypoint(entry);

  regs.gpr.dump();
  long long codeSize = memory.section("program").size;
  while(entry <= regs.pc.val.u && regs.pc.val.u < entry + codeSize) {
    stepi();
    if (cycleCount >= maxCycles) break;
  }
  fmt::print("pc:{:08x} (end)\n", regs.pc.val.u);
  regs.gpr.dump();
}

void Cpu::printPc()
{
  fmt::print("pc:{:08x}\n", regs.pc.val.u);
}
