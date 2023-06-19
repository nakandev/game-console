#include <memory>
#include <common.h>
#include <cpu.h>
#include <instruction.h>
#include <memory.h>

#include <fmt/core.h>

void
InstrRV32IManipulator::decodeTypeExtC(uint32_t bytes, Instruction& instr)
{
  // Instruction instr = {
  //   .opcode = (uint8_t)((bytes >>  0) & 0x7Fu),
  //   .funct3 = 0,
  //   .funct7 = 0,
  //   .dst    = (uint8_t)((bytes >>  7) & 0x1Fu),
  //   .src1   = 0,
  //   .src2   = 0,
  //   .imm    = sext(20,
  //       (((bytes >> 31) & 0x01u ) << 20) |
  //       (((bytes >> 21) & 0x3FFu) <<  1) |
  //       (((bytes >> 20) & 0x01u ) << 11) |
  //       (((bytes >> 12) & 0x0FFu) << 12)),
  //   .size = 4,
  //   .type = OPTYPE_J,
  // };
  instr = Instruction{.size=2};
}

void
InstrRV32IManipulator::executeTypeExtC(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.opcode == OPCODE_JAL) {
    // if (instr.dst != 0)
    //   regs.gpr[instr.dst].val.u = regs.pc.val.u + 4;
    // regs.pc.val.u += instr.imm.u;
    // instr.isJumped = true;
  } else {
    // unreachable
  }
}
void
InstrRV32IManipulator::decodeTypeCR(uint32_t bytes, Instruction& instr)
{
  instr = Instruction();
}
void InstrRV32IManipulator::executeTypeCR(Instruction& instr, RegisterSet& regs, Memory& memory)
{}
void
InstrRV32IManipulator::decodeTypeCI(uint32_t bytes, Instruction& instr)
{
  instr = Instruction();
}
void InstrRV32IManipulator::executeTypeCI(Instruction& instr, RegisterSet& regs, Memory& memory)
{}
void
InstrRV32IManipulator::decodeTypeCSS(uint32_t bytes, Instruction& instr)
{
  instr = Instruction();
}
void InstrRV32IManipulator::executeTypeCSS(Instruction& instr, RegisterSet& regs, Memory& memory)
{}
void
InstrRV32IManipulator::decodeTypeCIW(uint32_t bytes, Instruction& instr)
{
  instr = Instruction();
}
void InstrRV32IManipulator::executeTypeCIW(Instruction& instr, RegisterSet& regs, Memory& memory)
{}
void
InstrRV32IManipulator::decodeTypeCL(uint32_t bytes, Instruction& instr)
{
  instr = Instruction();
}
void InstrRV32IManipulator::executeTypeCL(Instruction& instr, RegisterSet& regs, Memory& memory)
{}
void
InstrRV32IManipulator::decodeTypeCS(uint32_t bytes, Instruction& instr)
{
  instr = Instruction();
}
void InstrRV32IManipulator::executeTypeCS(Instruction& instr, RegisterSet& regs, Memory& memory)
{}
void
InstrRV32IManipulator::decodeTypeCA(uint32_t bytes, Instruction& instr)
{
  instr = Instruction();
}
void InstrRV32IManipulator::executeTypeCA(Instruction& instr, RegisterSet& regs, Memory& memory)
{}
void
InstrRV32IManipulator::decodeTypeCB(uint32_t bytes, Instruction& instr)
{
  instr = Instruction();
}
void InstrRV32IManipulator::executeTypeCB(Instruction& instr, RegisterSet& regs, Memory& memory)
{}
void
InstrRV32IManipulator::decodeTypeCJ(uint32_t bytes, Instruction& instr)
{
  instr = Instruction();
}
void InstrRV32IManipulator::executeTypeCJ(Instruction& instr, RegisterSet& regs, Memory& memory)
{}
