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

uint8_t Cpu::loadElf(const string& path)
{
  elf = shared_ptr<Elf>(new Elf());
  if (elf.get()->load(path)) {
    return 1;
  }
  elf.get()->allocMemory(memory);
  programSection = &memory.section("program");
  auto& prg = memory.section("program");
  reset();
  return 0;
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
  regs.pc.val.u = elf.get()->getElfHeader().entry;

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

void Cpu::stepCycle()
{
  if (currentInstr[0].isWaiting) {
  } else
  {
    uint32_t val = programSection->read(regs.pc.val.u, 4);
    auto& instr = currentInstr[0];
    if (instr.phase == INSTR_PHASE_FETCH) {
      instrManip.fetch(instr);
    }
    if (instr.phase == INSTR_PHASE_DECODE) {
      instrManip.decode(val, instr);
      regs.prev_pc = regs.pc;
    }
    if (instr.phase == INSTR_PHASE_EXECUTE) {
      instrManip.execute(instr, regs, memory);
    }
    if (instr.phase == INSTR_PHASE_POSTPROC) {
      if (debugLevel >= 1) {
        if (regs.prev_pc.val.u != regs.pc.val.u) {
          instrManip.printInstr(instrCount, regs.prev_pc.val.u, instr, regs, memory);
        }
      }
      instr.phase = INSTR_PHASE_FETCH;
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

void Cpu::stepInstruction()
{
  auto& instr = currentInstr[0];
  while (instr.phase != 0) {
    stepCycle();
  }
}

const uint32_t Cpu::getPc()
{
  return regs.pc.val.u;
}

void Cpu::printPc()
{
  fmt::print("pc:{:08x}\n", regs.pc.val.u);
}

const vector<string> Cpu::disassembleAll()
{
  const uint8_t* buffer = programSection->buffer();
  size_t offset = 0;
  auto instr = Instruction();
  auto icount = 0;
  auto size = elf->getSection(".init")->size;
  size += elf->getSection(".text")->size;
  vector<string> disasms;
  while (offset < size) {
    auto baseAddr = programSection->addr;
    auto pc = baseAddr + offset;
    uint32_t val = programSection->read(pc, 4);

    instrManip.decode(val, instr);
    // instrManip.printInstr(icount, pc, instr, regs, memory);
    disasms.push_back(instrManip.instrToStr(icount, pc, instr, regs, memory));
    offset += instr.size;
    icount++;
  }
  return std::move(disasms);
}

const vector<string> Cpu::readRegisterAll()
{
  vector<string> regStrs;
  for (int i=0; i<regs.gpr.size(); i++) {
    string rs = fmt::format("gpr[{:2d}] {:08x}", i, regs.gpr[i].val.u);
    regStrs.push_back(rs);
  }
  return regStrs;
}
