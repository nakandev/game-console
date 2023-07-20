#include <memory>
#include <common.h>
#include <cpu.h>
#include <instruction.h>
#include <memory.h>

#include <fmt/core.h>

InstrRV32IManipulator::InstrRV32IManipulator()
{
}

InstrRV32IManipulator::~InstrRV32IManipulator()
{
}

void
InstrRV32IManipulator::decode(uint32_t bytes, Instruction& instr)
{
  if ((bytes & 0x3) != 3) {
    // 16-bits instruction
    uint32_t bytes32 = bytes;
    decodeTypeExtC(bytes32, instr);
  } else 
  if (((bytes >> 2) & 0x7) != 7) {
    // 32-bits instruction
    uint32_t bytes32 = bytes;
    uint8_t opcode = bytes32 & 0x7Fu;
    if (opcode == OPCODE_LOAD || opcode == OPCODE_STORE) {
      decodeTypeS(bytes32, instr);
    } else
    // if (opcode == OPCODE_ArithI || opcode == OPCODE_JALR || opcode == OPCODE_SYSTEM) {
    if (opcode == OPCODE_ArithI || opcode == OPCODE_JALR) {
      decodeTypeI(bytes32, instr);
    } else
    if (opcode == OPCODE_Arith) {
      decodeTypeR(bytes32, instr);
    } else
    if (opcode == OPCODE_BRANCH) {
      decodeTypeB(bytes32, instr);
    } else
    if (opcode == OPCODE_LUI || opcode == OPCODE_AUIPC) {
      decodeTypeU(bytes32, instr);
    } else
    if (opcode == OPCODE_JAL) {
      decodeTypeJ(bytes32, instr);
    } else
    if (opcode == OPCODE_SYSTEM) {
      decodeTypeSystem(bytes32, instr);
    } else
    {
      instr = Instruction{.size=4};
    }
  } else
  {
    instr = Instruction{.size=4};
  }
}

void
InstrRV32IManipulator::execute(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.prev_pc = regs.pc;
  if (instr.size == 2) {
    executeTypeExtC(instr, regs, memory);
  } else
  if (instr.size == 4) {
    if (instr.type == OPTYPE_R) {
      executeTypeR(instr, regs, memory);
    } else
    if (instr.type == OPTYPE_I) {
      executeTypeI(instr, regs, memory);
    } else
    if (instr.type == OPTYPE_S) {
      executeTypeS(instr, regs, memory);
    } else
    if (instr.type == OPTYPE_B) {
      executeTypeB(instr, regs, memory);
    } else
    if (instr.type == OPTYPE_U) {
      executeTypeU(instr, regs, memory);
    } else
    if (instr.type == OPTYPE_J) {
      executeTypeJ(instr, regs, memory);
    } else
    if (instr.type == OPTYPE_SYSTEM) {
      executeTypeSystem(instr, regs, memory);
    } else
    {}
  }
}


void
InstrRV32IManipulator::decodeTypeR(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x00u);
  instr.funct3 = (uint8_t)((bytes >> 12) & 0x07u);
  instr.funct7 = (uint8_t)((bytes >> 25) & 0x7Fu);
  instr.dst    = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src1   = (uint8_t)((bytes >> 15) & 0x1Fu);
  instr.src2   = (uint8_t)((bytes >> 20) & 0x1Fu);
  instr.imm.s    = 0;
  instr.size = 4;
  instr.type = OPTYPE_R;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void
InstrRV32IManipulator::executeTypeR(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.dst == 0) {
    return;
  }
  if (instr.funct7 == 0) {
    if (instr.funct3 == FUNCT3_ADD) {
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s + regs.gpr[instr.src2].val.s;
    } else
    if (instr.funct3 == FUNCT3_SLL) {
      regs.gpr[instr.dst].val.u = regs.gpr[instr.src1].val.u << regs.gpr[instr.src2].val.u;
    } else
    if (instr.funct3 == FUNCT3_SLT) {
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s < regs.gpr[instr.src2].val.s;
    } else
    if (instr.funct3 == FUNCT3_SLTU) {
      regs.gpr[instr.dst].val.u = regs.gpr[instr.src1].val.u < regs.gpr[instr.src2].val.u;
    } else
    if (instr.funct3 == FUNCT3_XOR) {
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s ^ regs.gpr[instr.src2].val.s;
    } else
    if (instr.funct3 == FUNCT3_SRL) {
      regs.gpr[instr.dst].val.u = regs.gpr[instr.src1].val.u >> regs.gpr[instr.src2].val.u;
    } else
    if (instr.funct3 == FUNCT3_OR) {
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s | regs.gpr[instr.src2].val.s;
    } else
    if (instr.funct3 == FUNCT3_AND) {
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s & regs.gpr[instr.src2].val.s;
    }
  } else
  if (instr.funct7 == 1) {
    if (instr.funct3 == FUNCT3_MUL) {
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s * regs.gpr[instr.src2].val.s;
    } else
    if (instr.funct3 == FUNCT3_MULH) {
      int64_t val = (int64_t)regs.gpr[instr.src1].val.s * regs.gpr[instr.src2].val.s;
      regs.gpr[instr.dst].val.s = (int32_t)(val >> 32);
    } else
    if (instr.funct3 == FUNCT3_MULHSU) {
      int64_t val = (int64_t)regs.gpr[instr.src1].val.s * regs.gpr[instr.src2].val.u;
      regs.gpr[instr.dst].val.s = (int32_t)(val >> 32);
    } else
    if (instr.funct3 == FUNCT3_MULHU) {
      uint64_t val = (uint64_t)regs.gpr[instr.src1].val.u * regs.gpr[instr.src2].val.u;
      regs.gpr[instr.dst].val.s = (uint32_t)(val >> 32);
    } else
    if (instr.funct3 == FUNCT3_DIV) {
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s / regs.gpr[instr.src2].val.s;
    } else
    if (instr.funct3 == FUNCT3_DIVU) {
      regs.gpr[instr.dst].val.u = regs.gpr[instr.src1].val.u / regs.gpr[instr.src2].val.u;
    } else
    if (instr.funct3 == FUNCT3_REM) {
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s % regs.gpr[instr.src2].val.s;
    } else
    if (instr.funct3 == FUNCT3_REMU) {
      regs.gpr[instr.dst].val.u = regs.gpr[instr.src1].val.u % regs.gpr[instr.src2].val.u;
    }
  } else
  if (instr.funct7 == 32) {
    if (instr.funct3 == FUNCT3_ADD) {  // SUB
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s - regs.gpr[instr.src2].val.s;
    } else
    if (instr.funct3 == FUNCT3_SRL) {  // SRA
        regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s >> regs.gpr[instr.src2].val.s;
    }
  } else
  {
  }
}

void
InstrRV32IManipulator::decodeTypeI(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x7Fu);
  instr.funct3 = (uint8_t)((bytes >> 12) & 0x07u);
  instr.funct7 = 0;
  instr.dst    = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src1   = (uint8_t)((bytes >> 15) & 0x1Fu);
  instr.src2   = 0;
  instr.imm.s    = sext(11, (bytes >> 20) & 0xFFFu);
  instr.size = 4;
  instr.type = OPTYPE_I;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void
InstrRV32IManipulator::executeTypeI(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.opcode == OPCODE_ArithI) {
    if (instr.dst == 0) {
      return;
    }
    if (instr.funct3 == FUNCT3_ADDI) {
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s + instr.imm.s;
    } else
    if (instr.funct3 == FUNCT3_SLTI) {
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s < instr.imm.s;
    } else
    if (instr.funct3 == FUNCT3_SLTIU) {
      regs.gpr[instr.dst].val.u = regs.gpr[instr.src1].val.u < instr.imm.u;
    } else
    if (instr.funct3 == FUNCT3_XORI) {
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s ^ instr.imm.s;
    } else
    if (instr.funct3 == FUNCT3_ORI) {
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s | instr.imm.s;
    } else
    if (instr.funct3 == FUNCT3_ANDI) {
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s & instr.imm.s;
    } else
    if (instr.funct3 == FUNCT3_SLLI) {
      int8_t imm = instr.imm.s & 0x1F;
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s << imm;
    } else
    if (instr.funct3 == FUNCT3_SRLI) {
      int8_t imm = instr.imm.s & 0x1F;
      int8_t mode = instr.imm.s >> 5;
      if (mode == 0b0000000) {
        regs.gpr[instr.dst].val.u = regs.gpr[instr.src1].val.u >> imm;
      } else
      if (mode == 0b0100000) {
        regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s >> imm;
      }
    } else
    {
    }
  } else
  if (instr.opcode == OPCODE_JALR) {
    if (instr.funct3 == FUNCT3_JALR) {
      // take care about instr.dst == instr.src1
      int32_t tmp = regs.gpr[instr.src1].val.s;
      if (instr.dst != 0) {
        regs.gpr[instr.dst].val.u = regs.pc.val.u + 4;
      }
      regs.pc.val.s = tmp + instr.imm.s;
      instr.isJumped = true;
    }
  } else
  if (instr.opcode == OPCODE_SYSTEM) {
    if (instr.funct3 == FUNCT3_PRIV) {
      if (instr.imm.s == FUNCT12_ECALL) {
      } else
      if (instr.imm.s == FUNCT12_EBREAK) {
      }
    }
  }
}

void
InstrRV32IManipulator::decodeTypeS(uint32_t bytes, Instruction& instr)
{
  uint8_t opcode = bytes & 0x7Fu;
  if (opcode == OPCODE_LOAD) {
    instr.binary = bytes;
    instr.opcode = (uint8_t)((bytes >>  0) & 0x7Fu);
    instr.funct3 = (uint8_t)((bytes >> 12) & 0x07u);
    instr.funct7 = 0;
    instr.dst    = (uint8_t)((bytes >>  7) & 0x1Fu);
    instr.src1   = (uint8_t)((bytes >> 15) & 0x1Fu);
    instr.src2   = 0;
    instr.imm.s    = sext(11, ((bytes >> 20) & 0xFFFu));
    instr.size = 4;
    instr.type = OPTYPE_S;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
  }
  else {
    instr.binary = bytes;
    instr.opcode = (uint8_t)((bytes >>  0) & 0x7Fu);
    instr.funct3 = (uint8_t)((bytes >> 12) & 0x07u);
    instr.funct7 = 0;
    instr.dst    = 0;
    instr.src1   = (uint8_t)((bytes >> 15) & 0x1Fu);
    instr.src2   = (uint8_t)((bytes >> 20) & 0x1Fu);
    instr.imm.s    = sext(11,
        (((bytes >> 25) & 0x7Fu) << 5) |
         ((bytes >>  7) & 0x1Fu)
        );
    instr.size = 4;
    instr.type = OPTYPE_S;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
  }
}
void
InstrRV32IManipulator::executeTypeS(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.opcode == OPCODE_LOAD) {
    // int size;
    // if      (instr.funct3 == FUNCT3_LB) size = 1;
    // else if (instr.funct3 == FUNCT3_LH) size = 2;
    // else if (instr.funct3 == FUNCT3_LW) size = 4;
    // else if (instr.funct3 == FUNCT3_LBU) size = 1;
    // else if (instr.funct3 == FUNCT3_LHU) size = 2;
    // uint32_t addr = regs.gpr[instr.src1].val.u + instr.imm.s;
    // if (instr.dst != 0) {
    //   regs.gpr[instr.dst].val.s = memory.read(addr, size);
    uint32_t addr = regs.gpr[instr.src1].val.u + instr.imm.s;
    if (instr.dst != 0) {
      if      (instr.funct3 == FUNCT3_LB)  regs.gpr[instr.dst].val.s = memory.read(addr, 1);
      else if (instr.funct3 == FUNCT3_LH)  regs.gpr[instr.dst].val.s = memory.read(addr, 2);
      else if (instr.funct3 == FUNCT3_LW)  regs.gpr[instr.dst].val.s = memory.read(addr, 4);
      else if (instr.funct3 == FUNCT3_LBU) regs.gpr[instr.dst].val.u = memory.read(addr, 1) & 0xffu;
      else if (instr.funct3 == FUNCT3_LHU) regs.gpr[instr.dst].val.u = memory.read(addr, 2) & 0xffffu;
      else fmt::print("Memory Load Error funct3={}\n", instr.funct3);
    }
  } else
  if (instr.opcode == OPCODE_STORE) {
    int size;
    if      (instr.funct3 == FUNCT3_SB) size = 1;
    else if (instr.funct3 == FUNCT3_SH) size = 2;
    else if (instr.funct3 == FUNCT3_SW) size = 4;
    uint32_t addr = regs.gpr[instr.src1].val.u + instr.imm.s;
    memory.write(addr, size, regs.gpr[instr.src2].val.s);
  }
}

void
InstrRV32IManipulator::decodeTypeB(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x7Fu);
  instr.funct3 = (uint8_t)((bytes >> 12) & 0x07u);
  instr.funct7 = 0;
  instr.dst    = 0;
  instr.src1   = (uint8_t)((bytes >> 15) & 0x1Fu);
  instr.src2   = (uint8_t)((bytes >> 20) & 0x1Fu);
  instr.imm.s    = sext(11,
      (((bytes >> 31) & 0x01u) << 12) |
      (((bytes >> 25) & 0x3Fu) <<  5) |
      (((bytes >>  8) & 0x0Fu) <<  1) |
      (((bytes >>  7) & 0x01u) << 11)
      );
  instr.size = 4;
  instr.type = OPTYPE_B;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void
InstrRV32IManipulator::executeTypeB(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  bool cond = false;
  if (instr.funct3 == FUNCT3_BEQ) {
    cond = regs.gpr[instr.src1].val.s == regs.gpr[instr.src2].val.s;
    if (cond) {
      regs.pc.val.u += instr.imm.u;
    }
  } else
  if (instr.funct3 == FUNCT3_BNE) {
    cond = regs.gpr[instr.src1].val.s != regs.gpr[instr.src2].val.s;
    if (cond) {
      regs.pc.val.u += instr.imm.u;
    }
  } else
  if (instr.funct3 == FUNCT3_BLT) {
    cond = regs.gpr[instr.src1].val.s < regs.gpr[instr.src2].val.s;
    if (cond) {
      regs.pc.val.u += instr.imm.u;
    }
  } else
  if (instr.funct3 == FUNCT3_BGE) {
    cond = regs.gpr[instr.src1].val.s >= regs.gpr[instr.src2].val.s;
    if (cond) {
      regs.pc.val.u += instr.imm.u;
    }
  } else
  if (instr.funct3 == FUNCT3_BLTU) {
    cond = regs.gpr[instr.src1].val.u < regs.gpr[instr.src2].val.u;
    if (cond) {
      regs.pc.val.u += instr.imm.u;
    }
  } else
  if (instr.funct3 == FUNCT3_BGEU) {
    cond = regs.gpr[instr.src1].val.u == regs.gpr[instr.src2].val.u;
    if (cond) {
      regs.pc.val.u += instr.imm.u;
    }
  } else {
    // unreachable
  }

  if (cond) {
    instr.isJumped = true;
  }
}

void
InstrRV32IManipulator::decodeTypeU(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x7Fu);
  instr.funct3 = 0;
  instr.funct7 = 0;
  instr.dst    = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src1   = 0;
  instr.src2   = 0;
  instr.imm.u    = bytes & 0xFFFFF000u;
  instr.size = 4;
  instr.type = OPTYPE_U;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void
InstrRV32IManipulator::executeTypeU(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.opcode == OPCODE_LUI) {
    if (instr.dst != 0)
      regs.gpr[instr.dst].val.u = instr.imm.u;
  } else
  if (instr.opcode == OPCODE_AUIPC) {
    if (instr.dst != 0)
      regs.gpr[instr.dst].val.u = regs.pc.val.u + instr.imm.u;
  } else {
    // unreachable
  }
}

void
InstrRV32IManipulator::decodeTypeJ(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x7Fu);
  instr.funct3 = 0;
  instr.funct7 = 0;
  instr.dst    = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src1   = 0;
  instr.src2   = 0;
  instr.imm.s    = sext(20,
      (((bytes >> 31) & 0x01u ) << 20) |
      (((bytes >> 21) & 0x3FFu) <<  1) |
      (((bytes >> 20) & 0x01u ) << 11) |
      (((bytes >> 12) & 0x0FFu) << 12));
  instr.size = 4;
  instr.type = OPTYPE_J;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void
InstrRV32IManipulator::executeTypeJ(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.opcode == OPCODE_JAL) {
    if (instr.dst != 0)
      regs.gpr[instr.dst].val.u = regs.pc.val.u + 4;
    regs.pc.val.u += instr.imm.u;
    instr.isJumped = true;
  } else {
    // unreachable
  }
}

void InstrRV32IManipulator::printInstrInfo(const Instruction& instr, const RegisterSet& regs, const Memory& memory)
{
  string type;
  string info;
  if (instr.size == 2) {
    fmt::print("16-bits op\n");
  } else
  if (instr.size == 4) {
    if (instr.type == OPTYPE_R) {
      type = "TypeR";
      info = fmt::format("dst:{:08x} = src1:{:08x} # src2:{:08x}",
        regs.gpr[instr.dst].val.u, regs.gpr[instr.src1].val.u, regs.gpr[instr.src2].val.u);
    } else
    if (instr.type == OPTYPE_I) {
      type = "TypeI";
      info = fmt::format("dst:{:08x} = src1:{:08x} # imm:{:08x}",
        regs.gpr[instr.dst].val.u, regs.gpr[instr.src1].val.u, instr.imm.u);
    } else
    if (instr.type == OPTYPE_S) {
      type = "TypeS";
      uint32_t addr = regs.gpr[instr.src1].val.u + instr.imm.s;
      int size = 4;
      if (instr.opcode == OPCODE_STORE) {
        info = fmt::format("memory.write({:08x},{:d},{:08x})",
            addr, size, regs.gpr[instr.src2].val.u);
      } else {
        info = fmt::format("reg[{:d}]{{{:08x}}} <= memory.read({:08x},{:d})",
            instr.dst, regs.gpr[instr.dst].val.u, addr, size);
      }
    } else
    if (instr.type == OPTYPE_B) {
      type = "TypeB";
    } else
    if (instr.type == OPTYPE_U) {
      type = "TypeU";
    } else
    if (instr.type == OPTYPE_J) {
      type = "TypeJ";
    } else
    {
      fmt::print("Unknown opcode: {:x}\n", instr.opcode);
    }
  } else
  {
    fmt::print("over 32-bit op\n");
  }
  fmt::print("pc:{:08x}  op:{:08x}  {:s} {:s}\n", regs.prev_pc.val.u, instr.binary, type, info);
}
