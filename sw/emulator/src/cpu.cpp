#include <cpu.h>
#include <memorymap.h>
#include <fmt/core.h>
#include <cassert>

int debugLevel = 0;

Cpu::Cpu(Memory& memory)
  : programSection(),
    elf(), isa(), currentInstr(),
    maxCycles(5000), cycleCount(), instrCount(),
    regs(),
    memory(memory)
{
  currentInstr.resize(1);
}

Cpu::~Cpu()
{
}

auto Cpu::setMaxCycles(int64_t cycles) -> void
{
  maxCycles = cycles;
}

auto Cpu::loadElf(const string& path) -> uint8_t
{
  elf = shared_ptr<Elf>(new Elf());
  if (elf.get()->load(path)) {
    return 1;
  }
  reset();
  cacheAllInstruction();
  return 0;
}

auto Cpu::init() -> void
{
  elf = shared_ptr<Elf>();
  memory.initMinimumSections();
  reset();
}

auto Cpu::reset() -> void
{
  for (auto &instr: currentInstr) {
    instr = Instruction();
  }
  regs.init();
  programSection = &memory.section("program");
  auto& stack = memory.section("stack");
  regs.gpr[2].val.u = stack.addr + stack.size - 4;
  if (elf.get()) {
    regs.pc.val.u = elf.get()->getElfHeader().entry;
    elf.get()->allocMemory(memory);
  } else {
    regs.pc.val.u = programSection->addr;
  }
  memory.processor = this;
  memory.prevProcessor = this;

  cycleCount = 0;
  instrCount = 0;
}

auto Cpu::requestInterruption() -> void
{
  regs.csr[MSTATUS].val.u |= 0x8;  // MIE bit on
}

auto Cpu::handleInterruption() -> void
{
  bool isIntrr = isa.checkInterruption(currentInstr[0], regs, memory);
  if (isIntrr && !memory.isBusy(0)) {
    isa.handleInterruption(currentInstr[0], regs);
  }
}

auto Cpu::stepCycle() -> void
{
  if (currentInstr[0].isWaiting) {
  } else
  {
    auto& instr = currentInstr[0];
    if (instr.phase == INSTR_PHASE_FETCH) {
      // if (!memory.isBusy(0)) {
        isa.fetch(instr);
      // }
    }
    uint32_t val = programSection->read32(regs.pc.val.u);
    if (instr.phase == INSTR_PHASE_DECODE) {
      if (regs.pc.val.s != regs.prev_pc.val.s) {
        isa.decode(val, instr);
        // instr = instrCache[regs.pc.val.u];
        regs.prev_pc = regs.pc;
      }
    }
    if (instr.phase == INSTR_PHASE_EXECUTE) {
      memory.prevProcessor = memory.processor;
      memory.processor = this;
      isa.execute(instr, regs, memory);
    }
    if (instr.phase == INSTR_PHASE_POSTPROC) {
      instrCount++;
      if (debugLevel >= 1) {
        if (regs.prev_pc.val.u != regs.pc.val.u) {
          isa.printInstr(instrCount, regs.prev_pc.val.u, instr, regs, memory);
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
}

auto Cpu::stepInstruction() -> void
{
  auto& instr = currentInstr[0];
  while (instr.phase != 0) {
    stepCycle();
  }
}

const auto Cpu::getPc() -> uint32_t
{
  return regs.pc.val.u;
}

const auto Cpu::disassembleAll() -> map<uint32_t, string>
{
  size_t offset = 0;
  auto instr = Instruction();
  auto icount = 0;
  auto size = elf->getSection(".init")->size;
  size += elf->getSection(".text")->size;
  // size += elf->getSection(".rodata")->size;
  // vector<string> disasms;
  map<uint32_t, string> disasms;
  while (offset < size) {
    auto baseAddr = programSection->addr;
    auto pc = baseAddr + offset;
    uint32_t val = programSection->read32(pc);

    isa.decode(val, instr);
    disasms[pc] = isa.instrToStr(icount, pc, instr, regs, memory);
    offset += instr.size;
    icount++;
  }
  return std::move(disasms);
}

const auto Cpu::readRegisterAll() -> vector<string>
{
  vector<string> regStrs;
  for (int i=0; i<regs.gpr.size(); i++) {
    string rs = fmt::format("gpr[{:2d}] {:08x}", i, regs.gpr[i].val.u);
    regStrs.push_back(rs);
  }
  return regStrs;
}

auto Cpu::cacheAllInstruction() -> void
{
  regs.pc.val.u = elf.get()->getElfHeader().entry;
  uint32_t prgEnd = programSection->addr + programSection->size;
  int count = 0;
  instrCache.clear();
  while (regs.pc.val.u < prgEnd && count < 1000000) {
    Instruction instr;
    uint32_t val = programSection->read32(regs.pc.val.u);
    isa.decode(val, instr);
    instrCache[regs.pc.val.u] = instr;
    regs.pc.val.u += instr.size;
    count++;
  }
  regs.pc.val.u = elf.get()->getElfHeader().entry;
}

auto Cpu::getInstrCount() -> int64_t
{
  return instrCount;
}

auto Cpu::getCycleCount() -> int64_t
{
  return cycleCount;
}
