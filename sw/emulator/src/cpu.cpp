#include <cpu.h>

#include <fmt/core.h>
#include <cassert>

int debugLevel = 0;

Cpu::Cpu(Memory& memory)
  : programSection(),
    elf(), instrManip(), currentInstr(),
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

void Cpu::loadElf(const string& path)
{
  elf = shared_ptr<Elf>(new Elf());
  elf.get()->load(path);
  elf.get()->allocMemory(memory);
  programSection = &memory.section("program");
  auto& prg = memory.section("program");
  reset();
}

void Cpu::init()
{
  reset();
  elf = shared_ptr<Elf>();
  memory.initMinimumSections();
}

void Cpu::reset()
{
  for (auto &instr: currentInstr) {
    instr = Instruction();
  }
  regs.init();
  auto& stack = memory.section("stack");
  regs.gpr[2].val.u = stack.addr + stack.size - 4;
  setEntrypoint(elf.get()->getElfHeader().entry);

  cycleCount = 0;
  instrCount = 0;
}

void Cpu::requestInterruption()
{
  regs.csr[MSTATUS].val.u |= 0x8;  // MIE bit on
}

void Cpu::handleInterruption()
{
  bool isIntrr = instrManip.checkInterruption(currentInstr[0], regs, memory);
  if (isIntrr)
  {
    instrManip.handleInterruption(currentInstr[0], regs);
  }
}

void Cpu::stepi()
{
  if (currentInstr[0].isWaiting) {
  } else
  {
    uint32_t val = programSection->read(regs.pc.val.u, 4);
    auto& instr = currentInstr[0];
    if (instr.phase == 0) {
      // fetch
      instr.phase = 1;
    }
    if (instr.phase == 1) {
      instrManip.decode(val, instr);
    }
    if (instr.phase == 2) {
      instrManip.execute(instr, regs, memory);
    }
    if (instr.phase == 3) {
      // post execute
      if (debugLevel >= 1) {
        if (regs.prev_pc.val.u != regs.pc.val.u) {
          instrManip.printInstr(instrCount, regs.prev_pc.val.u, instr, regs, memory);
        }
      }
      instr.phase = 0;
    }
  }
  if (debugLevel >= 1) {
    if (cycleCount >= maxCycles) {
      exit(0);
    }
  }
  cycleCount++;
  instrCount++;
}

void Cpu::setEntrypoint(uint32_t entry)
{
  regs.pc.val.u = entry;
}

void Cpu::launch()
{
  reset();

  regs.gpr.dump();
  uint32_t entry = regs.pc.val.u;
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

void Cpu::disassembleAll()
{
  const uint8_t* buffer = programSection->buffer();
  size_t offset = 0;
  auto instr = Instruction();
  auto icount = 0;
  auto size = elf->getSection(".init")->size;
  size += elf->getSection(".text")->size;
  fmt::print("size={}\n", size);
  while (offset < size) {
    auto baseAddr = programSection->addr;
    auto pc = baseAddr + offset;
    uint32_t val = programSection->read(pc, 4);

    instrManip.decode(val, instr);
    instrManip.printInstr(icount, pc, instr, regs, memory);
    offset += instr.size;
    icount++;
  }
}
