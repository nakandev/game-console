#include <memory>
#include <common.h>
#include <cpu.h>
#include <instruction.h>
#include <memory.h>

#include <fmt/core.h>

const string
InstrRV32IManipulator::instrToStr(int64_t icount, uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  auto op = disassemble(pc, instr, regs, memory);
  auto reginfo = fmt::format("x{:02d}={:08x} x{:02d}={:08x} x{:02d}={:08x} imm={:08x}",
    instr.dst , regs.gpr[instr.dst].val.u,
    instr.src1, regs.gpr[instr.src1].val.u,
    instr.src2, regs.gpr[instr.src2].val.u,
    instr.imm.u
  );
  string space;
  string opinfo;
  if (instr.size == 2) {
    space = "    ";
    // opinfo = fmt::format("{} {:08x}: {:04x}{}  {}", icount, pc, instr.binary, space, op);
    opinfo = fmt::format("{:08x}: {:04x}{}  {}", pc, instr.binary, space, op);
  } else
  if (instr.size == 4) {
    space = "";
    // opinfo = fmt::format("{} {:08x}: {:08x}{}  {}", icount, pc, instr.binary, space, op);
    opinfo = fmt::format("{:08x}: {:08x}{}  {}", pc, instr.binary, space, op);
  }
  string printstr = fmt::format("{}", opinfo);
  auto tablen = 64 - opinfo.size();
  for (int i=0; i<tablen; i++) {
    printstr += fmt::format(" ");
  }
  // printstr += fmt::format("  {}", reginfo);
  printstr += fmt::format("\n");
  // fmt::print("{}", printstr);
  return std::move(printstr);
}

void
InstrRV32IManipulator::printInstr(int64_t icount, uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string s = instrToStr(icount, pc, instr, regs, memory);
  fmt::print("{}", s);
}

string
InstrRV32IManipulator::disassemble(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string op = "???";
  if (instr.size == 2) {
    // 16-bits instruction
    uint8_t optype = instr.type;
    if (optype == OPTYPE_CR) {
      op = disassembleTypeCR(pc, instr, regs, memory);
    } else
    if (optype == OPTYPE_CI) {
      op = disassembleTypeCI(pc, instr, regs, memory);
    } else
    if (optype == OPTYPE_CSS) {
      op = disassembleTypeCSS(pc, instr, regs, memory);
    } else
    if (optype == OPTYPE_CIW) {
      op = disassembleTypeCIW(pc, instr, regs, memory);
    } else
    if (optype == OPTYPE_CL) {
      op = disassembleTypeCL(pc, instr, regs, memory);
    } else
    if (optype == OPTYPE_CS) {
      op = disassembleTypeCS(pc, instr, regs, memory);
    } else
    if (optype == OPTYPE_CA) {
      op = disassembleTypeCA(pc, instr, regs, memory);
    } else
    if (optype == OPTYPE_CB2) {
      op = disassembleTypeCB2(pc, instr, regs, memory);
    } else
    if (optype == OPTYPE_CB) {
      op = disassembleTypeCB(pc, instr, regs, memory);
    } else
    if (optype == OPTYPE_CJ) {
      op = disassembleTypeCJ(pc, instr, regs, memory);
    }
  } else 
  if (instr.size == 4) {
    // 32-bits instruction
    uint8_t optype = instr.type;
    if (optype == OPTYPE_S) {
      op = disassembleTypeS(pc, instr, regs, memory);
    } else
    if (optype == OPTYPE_I) {
      op = disassembleTypeI(pc, instr, regs, memory);
    } else
    if (optype == OPTYPE_R) {
      op = disassembleTypeR(pc, instr, regs, memory);
    } else
    if (optype == OPTYPE_B) {
      op = disassembleTypeB(pc, instr, regs, memory);
    } else
    if (optype == OPTYPE_U) {
      op = disassembleTypeU(pc, instr, regs, memory);
    } else
    if (optype == OPTYPE_J) {
      op = disassembleTypeJ(pc, instr, regs, memory);
    } else
    if (optype == OPTYPE_SYSTEM) {
      op = disassembleTypeSystem(pc, instr, regs, memory);
    }
  } else
  {
  }
  return std::move(op);
}

string
InstrRV32IManipulator::disassembleTypeR(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string dst = fmt::format("x{}", instr.dst);
  string src1 = fmt::format("x{}", instr.src1);
  string src2 = fmt::format("x{}", instr.src2);
  string opcode = "<typeR>";
  switch (instr.instr) {
    case INSTR_ADD   : opcode = "add"; break;
    case INSTR_SLL   : opcode = "sll"; break;
    case INSTR_SLT   : opcode = "slt"; break;
    case INSTR_SLTU  : opcode = "sltu"; break;
    case INSTR_XOR   : opcode = "xor"; break;
    case INSTR_SRL   : opcode = "srl"; break;
    case INSTR_OR    : opcode = "or"; break;
    case INSTR_AND   : opcode = "and"; break;
    case INSTR_MUL   : opcode = "mul"; break;
    case INSTR_MULH  : opcode = "mulh"; break;
    case INSTR_MULHSU: opcode = "mulhsu"; break;
    case INSTR_MULHU : opcode = "mulhu"; break;
    case INSTR_DIV   : opcode = "div"; break;
    case INSTR_DIVU  : opcode = "divu"; break;
    case INSTR_REM   : opcode = "rem"; break;
    case INSTR_REMU  : opcode = "remu"; break;
    case INSTR_SUB   : opcode = "sub"; break;
    case INSTR_SRA   : opcode = "sra"; break;
    default: break;
  }
  string op = fmt::format("{} {}, {}, {}", opcode, dst, src1, src2);
  return std::move(op);
}

string
InstrRV32IManipulator::disassembleTypeI(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string dst = fmt::format("x{}", instr.dst);
  string src1 = fmt::format("x{}", instr.src1);
  string imm = fmt::format("0x{:x}<{}>", instr.imm.s, instr.imm.s);
  string opcode = "<typeI>";
  switch (instr.instr) {
    case INSTR_ADDI  : opcode = "addi"; break;
    case INSTR_SLTI  : opcode = "slti"; break;
    case INSTR_SLTIU : opcode = "sltiu"; break;
    case INSTR_XORI  : opcode = "xori"; break;
    case INSTR_ORI   : opcode = "ori"; break;
    case INSTR_ANDI  : opcode = "andi"; break;
    case INSTR_SLLI  : opcode = "slli"; break;
    case INSTR_SRLI  : opcode = "srli"; break;
    case INSTR_SRAI  : opcode = "srai"; break;
    case INSTR_JALR  : opcode = "jalr"; break;
    default: break;
  }
  string op = fmt::format("{} {}, {}, {}", opcode, dst, src1, imm);
  return std::move(op);
}

string
InstrRV32IManipulator::disassembleTypeS(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string dst = fmt::format("x{}", instr.dst);
  string src1 = fmt::format("x{}", instr.src1);
  string imm = fmt::format("0x{:x}<{}>", instr.imm.u, instr.imm.u);
  string opcode = "<typeS>";
  switch (instr.instr) {
    case INSTR_LB    : opcode = "lb"; break;
    case INSTR_LH    : opcode = "lh"; break;
    case INSTR_LW    : opcode = "lw"; break;
    case INSTR_LBU   : opcode = "lbu"; break;
    case INSTR_LHU   : opcode = "lhu"; break;
    case INSTR_SB    : opcode = "sb"; break;
    case INSTR_SH    : opcode = "sh"; break;
    case INSTR_SW    : opcode = "sw"; break;
    default: break;
  }
  string op = fmt::format("{} {}, {} ({})", opcode, dst, imm, src1);
  return std::move(op);
}

string
InstrRV32IManipulator::disassembleTypeU(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string dst = fmt::format("x{}", instr.dst);
  string imm = fmt::format("0x{:x}<{}>", instr.imm.u, instr.imm.u);
  string opcode = "<typeU>";
  switch (instr.instr) {
    case INSTR_LUI   : opcode = "lui"; break;
    case INSTR_AUIPC : opcode = "auipc"; break;
    default: break;
  }
  string op = fmt::format("{} {}, {}", opcode, dst, imm);
  return std::move(op);
}

string
InstrRV32IManipulator::disassembleTypeB(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string src1 = fmt::format("x{}", instr.src1);
  string src2 = fmt::format("x{}", instr.src2);
  string imm = fmt::format("0x{:x}<{}>", instr.imm.u, instr.imm.u);
  string opcode = "<typeB>";
  switch (instr.instr) {
    case INSTR_BEQ   : opcode = "beq"; break;
    case INSTR_BNE   : opcode = "bne"; break;
    case INSTR_BLT   : opcode = "blt"; break;
    case INSTR_BGE   : opcode = "bge"; break;
    case INSTR_BLTU  : opcode = "bltu"; break;
    case INSTR_BGEU  : opcode = "bgeu"; break;
    default: break;
  }
  string op = fmt::format("{} {}, {}, {}", opcode, src1, src2, imm);
  return std::move(op);
}

string
InstrRV32IManipulator::disassembleTypeJ(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string dst = fmt::format("x{}", instr.dst);
  string src1 = fmt::format("x{}", instr.src1);
  string src2 = fmt::format("x{}", instr.src2);
  string opcode = "<typeJ>";
  switch (instr.instr) {
    case INSTR_JAL   : opcode = "jal"; break;
    default: break;
  }
  string op = fmt::format("{} {}, {}, {}", opcode, dst, src1, src2);
  return std::move(op);
}

string
InstrRV32IManipulator::disassembleTypeSystem(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string opcode = "<typeSystem>";
  if (instr.instr == INSTR_ECALL) {
    string op = fmt::format("ecall");
    return std::move(op);
  } else
  if (instr.instr == INSTR_EBREAK) {
    string op = fmt::format("ebreak");
    return std::move(op);
  } else
  if (instr.instr == INSTR_MRET) {
    string op = fmt::format("mret");
    return std::move(op);
  } else
  if (instr.instr == INSTR_WFI) {
    string op = fmt::format("wfi");
    return std::move(op);
  } else {
    string dst = fmt::format("x{}", instr.dst);
    string csr = fmt::format("0x{}", instr.imm.u);
    string src1 = fmt::format("x{}", instr.src1);
    string opcode = "<typeSystem>";
    // switch (instr.instr) {
    //   case INSTR_ADD   : opcode = "add     "; break;
    //   default: break;
    // }
    string op = fmt::format("{} {}, {}, {}", opcode, dst, csr, src1);
    return std::move(op);
  }
}

static const uint8_t tbl[8] = {
  8, 9, 10, 11, 12, 13, 14, 15
};

string
InstrRV32IManipulator::disassembleTypeCR(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string dst = fmt::format("x{}", instr.dst);
  string src1 = fmt::format("x{}", instr.src1);
  string src2 = fmt::format("x{}", instr.src2);
  string opcode = "???";
  switch (instr.instr) {
    case INSTR_CJR     : opcode = "c.jr"; break;
    case INSTR_CMV     : opcode = "c.mv"; break;
    case INSTR_CEBREAK : opcode = "c.ebreak"; break;
    case INSTR_CJALR   : opcode = "c.jalr"; break;
    case INSTR_CADD    : opcode = "c.add"; break;
    default: break;
  }
  string op = fmt::format("{} {}, {}, {}", opcode, dst, src1, src2);
  return std::move(op);
}
string
InstrRV32IManipulator::disassembleTypeCI(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string dst = fmt::format("x{}", instr.dst);
  string src1 = fmt::format("x{}", instr.src1);
  string imm = fmt::format("0x{:x}({})", instr.imm.s, instr.imm.s);
  string opcode = "<typeCI>";
  string op;
  if (instr.instr == INSTR_CLWSP) {
    opcode = "c.lwsp";
    op = fmt::format("{} {}, {} ({})", opcode, dst, imm, src1);
  } else
  {
    switch (instr.instr) {
      case INSTR_CLWSP     : opcode = "c.lwsp"; break;
      case INSTR_CSLLI     : opcode = "c.slli"; break;
      case INSTR_CLI       : opcode = "c.li"; break;
      case INSTR_CADDI16SP : opcode = "c.addi16sp"; break;
      case INSTR_CLUI      : opcode = "c.lui"; break;
      case INSTR_CADDI     : opcode = "c.addi"; break;
      case INSTR_CNOP      : opcode = "c.nop"; break;
      default: break;
    }
    op = fmt::format("{} {}, {}, {}", opcode, dst, src1, imm);
  }
  return std::move(op);
}
string
InstrRV32IManipulator::disassembleTypeCSS(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string dst = fmt::format("x{}", 2);
  string src2 = fmt::format("x{}", instr.src2);
  string imm = fmt::format("0x{:x}<{}>", instr.imm.s, instr.imm.s);
  string opcode = "<typeCSS>";
  switch (instr.instr) {
    case INSTR_CSWSP : opcode = "c.swsp"; break;
    default: break;
  }
  string op = fmt::format("{} {}, {} ({})", opcode, dst, imm, src2);
  return std::move(op);
}
string
InstrRV32IManipulator::disassembleTypeCIW(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string dst = fmt::format("x{}", tbl[instr.dst]);
  string src1 = fmt::format("x{}", 2);
  string imm = fmt::format("0x{:x}<{}>", instr.imm.s, instr.imm.s);
  string opcode = "<typeCIW>";
  switch (instr.instr) {
    case INSTR_CADDI4SPN : opcode = "c.addi4spn"; break;
    default: break;
  }
  string op = fmt::format("{} {}, {}, ({})", opcode, dst, imm, src1);
  return std::move(op);
}
string
InstrRV32IManipulator::disassembleTypeCL(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string src1 = fmt::format("x{}", tbl[instr.src1]);
  string dst = fmt::format("x{}", tbl[instr.dst]);
  string imm = fmt::format("0x{:x}<{}>", instr.imm.s, instr.imm.s);
  string opcode = "<typeCL>";
  switch (instr.instr) {
    case INSTR_CLW : opcode = "c.lw"; break;
    default: break;
  }
  string op = fmt::format("{} {}, {}, ({})", opcode, dst, imm, src1);
  return std::move(op);
}
string
InstrRV32IManipulator::disassembleTypeCS(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string src1 = fmt::format("x{}", tbl[instr.src1]);
  string src2 = fmt::format("x{}", tbl[instr.src2]);
  string imm = fmt::format("0x{:x}<{}>", instr.imm.s, instr.imm.s);
  string opcode = "<typeCS>";
  switch (instr.instr) {
    case INSTR_CSW : opcode = "c.sw"; break;
    default: break;
  }
  string op = fmt::format("{} {}, {}, ({})", opcode, src2, imm, src1);
  return std::move(op);
}
string
InstrRV32IManipulator::disassembleTypeCA(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string dst = fmt::format("x{}", tbl[instr.dst]);
  string src1 = fmt::format("x{}", tbl[instr.src1]);
  string src2 = fmt::format("x{}", tbl[instr.src2]);
  string opcode = "<typeCA>";
  switch (instr.instr) {
    case INSTR_CSUB : opcode = "c.sub"; break;
    case INSTR_CXOR : opcode = "c.xor"; break;
    case INSTR_COR  : opcode = "c.or"; break;
    case INSTR_CAND : opcode = "c.and"; break;
    default: break;
  }
  string op = fmt::format("{} {}, {}, {}", opcode, dst, src1, src2);
  return std::move(op);
}
string
InstrRV32IManipulator::disassembleTypeCB2(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string dst = fmt::format("x{}", tbl[instr.dst]);
  string src1 = fmt::format("x{}", tbl[instr.src1]);
  string imm = fmt::format("0x{:x}<{}>", instr.imm.s, instr.imm.s);
  string opcode = "<typeCB2>";
  switch (instr.instr) {
    case INSTR_CSRLI : opcode = "c.srli"; break;
    case INSTR_CSRAI : opcode = "c.srai"; break;
    case INSTR_CANDI : opcode = "c.andi"; break;
    default: break;
  }
  string op = fmt::format("{} {}, {}, {}", opcode, dst, src1, imm);
  return std::move(op);
}
string
InstrRV32IManipulator::disassembleTypeCB(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  string dst = fmt::format("x{}", tbl[instr.dst]);
  string src1 = fmt::format("x{}", tbl[instr.src1]);
  string imm = fmt::format("0x{:x}<{}>", instr.imm.s, instr.imm.s);
  string opcode = "<typeCB>";
  switch (instr.instr) {
    case INSTR_CBEQZ : opcode = "c.beqz"; break;
    case INSTR_CBNEZ : opcode = "c.bnez"; break;
    default: break;
  }
  string op = fmt::format("{} {}, {}, {}", opcode, dst, src1, imm);
  return std::move(op);
}
string
InstrRV32IManipulator::disassembleTypeCJ(uint32_t pc, Instruction& instr, RegisterSet& regs, Memory& memory)
{
  uint8_t dstreg = instr.instr == INSTR_CJAL ? 1 : 0;
  string dst = fmt::format("x{}", dstreg);
  string imm = fmt::format("0x{:x}<{}>", instr.imm.s, instr.imm.s);
  string opcode = "<typeCJ>";
  switch (instr.instr) {
    case INSTR_CJAL : opcode = "c.jal"; break;
    case INSTR_CJ   : opcode = "c.j"; break;
    default: break;
  }
  string op = fmt::format("{} {}, {}", opcode, dst, imm);
  return std::move(op);
}
