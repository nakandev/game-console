#include <memory>
#include <common.h>
#include <cpu.h>
#include <instruction.h>
#include <memory.h>

#include <fmt/core.h>

void
InstrRV32IManipulator::decodeTypeExtC(uint32_t bytes, Instruction& instr)
{
  uint16_t bytes16 = bytes & 0xFFFFu;
  uint8_t opcode = bytes16 & 0x3u;
  uint8_t funct3 = (bytes16 >> 13) & 0x7u;
  if (opcode == OPCODE_C2 && funct3 == 0b100) {
    decodeTypeCR(bytes16, instr);
  } else
  if (funct3 == 0b000 || funct3 == 0b001 || funct3 == 0b010 || funct3 == 0b011) {
    if (opcode == OPCODE_C0) {
      decodeTypeCIW(bytes16, instr);
    }
    else {
      decodeTypeCI(bytes16, instr);
    }
  } else
  if (opcode == OPCODE_C2 && (funct3 == 0b101 || funct3 == 0b110 || funct3 == 0b111)) {
    decodeTypeCSS(bytes16, instr);
  } else
  if (opcode == OPCODE_C0 && (funct3 == 0b001 || funct3 == 0b010 || funct3 == 0b011)) {
    decodeTypeCL(bytes16, instr);
  } else
  if (opcode == OPCODE_C0 && (funct3 == 0b101 || funct3 == 0b110 || funct3 == 0b111)) {
    decodeTypeCS(bytes16, instr);
  } else
  if (opcode == OPCODE_C1 && funct3 == 0b100) {
    uint8_t funct2 = (bytes16 >> 10) & 0x3u;
    if (funct2 == 0b11) {
      decodeTypeCA(bytes16, instr);
    }
    else {
      decodeTypeCB2(bytes16, instr);
    }
  } else
  if (opcode == OPCODE_C1 && (funct3 == 0b110 || funct3 == 0b111)) {
    decodeTypeCB(bytes16, instr);
  } else
  if (opcode == OPCODE_C1 && (funct3 == 0b001 || funct3 == 0b101)) {
    decodeTypeCJ(bytes16, instr);
  } else
  {
    instr = Instruction{.size=2};
  }
}

void
InstrRV32IManipulator::executeTypeExtC(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.type == OPTYPE_CR) {
    executeTypeCR(instr, regs, memory);
  } else
  if (instr.type == OPTYPE_CI) {
    executeTypeCI(instr, regs, memory);
  } else
  if (instr.type == OPTYPE_CSS) {
    executeTypeCSS(instr, regs, memory);
  } else
  if (instr.type == OPTYPE_CIW) {
    executeTypeCIW(instr, regs, memory);
  } else
  if (instr.type == OPTYPE_CL) {
    executeTypeCL(instr, regs, memory);
  } else
  if (instr.type == OPTYPE_CS) {
    executeTypeCS(instr, regs, memory);
  } else
  if (instr.type == OPTYPE_CA) {
    executeTypeCA(instr, regs, memory);
  } else
  if (instr.type == OPTYPE_CB2) {
    executeTypeCB2(instr, regs, memory);
  } else
  if (instr.type == OPTYPE_CB) {
    executeTypeCB(instr, regs, memory);
  } else
  if (instr.type == OPTYPE_CJ) {
    executeTypeCJ(instr, regs, memory);
  } else
  {
    // unreachable
  }
}

static const uint8_t tbl[8] = {
  8, 9, 10, 11, 12, 13, 14, 15
};

void
InstrRV32IManipulator::decodeTypeCR(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x03u);
  instr.funct3 = (uint8_t)((bytes >> 12) & 0x0Fu);  // funct4
  instr.funct7 = (uint8_t)((bytes >>  5) & 0x03u);  // funct2
  instr.dst    = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src1   = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src2   = (uint8_t)((bytes >>  2) & 0x1Fu);
  instr.imm.s    = 0;
  instr.size = 2;
  instr.type = OPTYPE_CR;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void InstrRV32IManipulator::executeTypeCR(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.funct3 == 0b1000) {
    if (instr.src2 == 0) {
      // c.jr
      if (instr.src1 != 0) {
        regs.pc.val.s = regs.gpr[instr.src1].val.s + 0;
        instr.isJumped = true;
      }
    } else
    {
      // c.mv
      if (instr.src1 != 0) {
        regs.gpr[instr.dst].val.s = regs.gpr[instr.src2].val.s;
      }
    }
  } else
  if (instr.funct3 == 0b1001) {
    if (instr.src2 == 0 && instr.src1 == 0) {
      // c.ebreak
    } else
    if (instr.src2 == 0) {
      // c.jalr
      regs.gpr[1].val.u = regs.pc.val.u + 4;
      regs.pc.val.s = regs.gpr[instr.src1].val.s + 0;
      instr.isJumped = true;
    } else
    if (instr.src2 != 0 && instr.src1 != 0) {
      // c.add
      regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s + regs.gpr[instr.src2].val.s;
    }
  }
}

void
InstrRV32IManipulator::decodeTypeCI(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x03u);
  instr.funct3 = (uint8_t)((bytes >> 13) & 0x07u);  // funct3
  instr.dst    = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src1   = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src2   = 0;
  instr.imm.s    = sext(7,
      (((bytes >> 12) & 0x01u) << 12) |
      (((bytes >>  4) & 0x07u) <<  2) |
      (((bytes >>  2) & 0x03u) <<  6)
      );
  instr.size = 2;
  instr.type = OPTYPE_CI;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void InstrRV32IManipulator::executeTypeCI(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.opcode == OPCODE_C2) {
    if (instr.funct3 == 0b010) {
      // c.lwsp
      uint32_t addr = regs.gpr[2].val.u + instr.imm.s;
      regs.gpr[tbl[instr.dst]].val.s = memory.read(addr, 4);
    } else
    if (instr.funct3 == 0b000) {
      // c.slli
      regs.gpr[tbl[instr.dst]].val.s = regs.gpr[tbl[instr.src1]].val.s << instr.imm.s;
    } else
    {
      // c.ldsp, c.lqsp, c.flwsp, c.fldsp (rv64/rv128 only)
    }
  } else
  if (instr.opcode == OPCODE_C1) {
    int32_t imm = instr.imm.s;
    if (instr.funct3 == 0b010) {
      // c.li
      if (tbl[instr.dst] != 0) {
        regs.gpr[tbl[instr.dst]].val.s = imm;
      }
    } else
    if (instr.funct3 == 0b011) {
      if (tbl[instr.dst] == 0) {
        // hint
      } else
      if (tbl[instr.dst] == 2) {
        // c.addi16sp
        regs.gpr[tbl[instr.dst]].val.s = regs.gpr[tbl[instr.src1]].val.s + imm;
      } else
      {
        // c.lui
        if (imm != 0) {
          regs.gpr[tbl[instr.dst]].val.s = imm << 12;
        } else
        {
          // error: imm can be non-zero value.
        }
      }
    } else
    if (instr.funct3 == 0b000) {
      if (tbl[instr.dst] != 0) {
        // c.addi
        int32_t newimm = imm;
        regs.gpr[tbl[instr.dst]].val.s = regs.gpr[tbl[instr.src1]].val.s + newimm;
      } else
      {
        // c.nop
      }
    } else
    if (instr.funct3 == 0b001) {
      // c.addiw (rv64/rv128 only)
    }
  }
}

void
InstrRV32IManipulator::decodeTypeCSS(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x03u);
  instr.funct3 = (uint8_t)((bytes >> 13) & 0x07u);
  instr.dst    = 0;
  instr.src1   = 0;
  instr.src2   = (uint8_t)((bytes >>  2) & 0x1Fu);
  instr.imm.s    = sext(6, ((bytes >> 7) & 0x3Fu));
  instr.size = 2;
  instr.type = OPTYPE_CSS;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void InstrRV32IManipulator::executeTypeCSS(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.opcode == OPCODE_C2) {
    if (instr.funct3 == 0b110) {
      // c.swsp
      uint32_t addr = regs.gpr[tbl[instr.src2]].val.u + instr.imm.s;
      memory.write(addr, 4, regs.gpr[tbl[instr.dst]].val.s);
    } else
    {
      // c.sdsp, c.sqsp, c.fswsp, c.fsdsp (rv64/rv128 only)
    }
  }
}

void
InstrRV32IManipulator::decodeTypeCIW(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x00u);
  instr.funct3 = (uint8_t)((bytes >> 13) & 0x07u);
  instr.dst    = (uint8_t)((bytes >>  2) & 0x07u);
  instr.src1   = 0;
  instr.src2   = 0;
  instr.imm.s    = sext(8, ((bytes >> 5) & 0xFFu));
  instr.size = 2;
  instr.type = OPTYPE_CIW;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void InstrRV32IManipulator::executeTypeCIW(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  // c.addi4spn
  regs.gpr[tbl[instr.dst]].val.s = regs.gpr[2].val.s + instr.imm.s * 4;
}

void
InstrRV32IManipulator::decodeTypeCL(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x03u);
  instr.funct3 = (uint8_t)((bytes >> 13) & 0x07u);
  instr.dst    = (uint8_t)((bytes >>  2) & 0x07u);
  instr.src1   = (uint8_t)((bytes >>  7) & 0x07u);
  instr.src2   = 0;
  instr.imm.s    = sext(6,
      (((bytes >> 10) & 0x07u) << 3) |
      (((bytes >>  6) & 0x01u) << 2) |
      (((bytes >>  5) & 0x01u) << 6)
      );
  instr.size = 2;
  instr.type = OPTYPE_CL;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void InstrRV32IManipulator::executeTypeCL(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.funct3 == 0b010) {
    // c.lw
      uint32_t addr = regs.gpr[tbl[instr.src2]].val.u + instr.imm.s;
      regs.gpr[tbl[instr.dst]].val.s = memory.read(addr, 4);
  } else
  if (instr.funct3 == 0b011) {
    // c.flw
    // c.ld (rv64/rv128 only)
  } else
  if (instr.funct3 == 0b001) {
    // c.fld (rv64/rv128 only)
    // c.lq (rv64/rv128 only)
  }
}

void
InstrRV32IManipulator::decodeTypeCS(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x03u);
  instr.funct3 = (uint8_t)((bytes >> 13) & 0x07u);
  instr.dst    = 0;
  instr.src1   = (uint8_t)((bytes >>  7) & 0x07u);
  instr.src2   = (uint8_t)((bytes >>  2) & 0x07u);
  instr.imm.s    = sext(6,
      (((bytes >> 10) & 0x07u) << 3) |
      (((bytes >>  6) & 0x01u) << 2) |
      (((bytes >>  5) & 0x01u) << 6)
      );
  instr.size = 2;
  instr.type = OPTYPE_CS;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void InstrRV32IManipulator::executeTypeCS(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.funct3 == 0b110) {
    // c.sw
      uint32_t addr = regs.gpr[tbl[instr.src2]].val.u + instr.imm.s;
      memory.write(addr, 4, regs.gpr[tbl[instr.dst]].val.s);
  } else
  if (instr.funct3 == 0b111) {
    // c.fsw
    // c.sd (rv64/rv128 only)
  } else
  if (instr.funct3 == 0b101) {
    // c.fsd (rv64/rv128 only)
    // c.sq (rv64/rv128 only)
  }
}

void
InstrRV32IManipulator::decodeTypeCA(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x03u);
  instr.funct3 = (uint8_t)((bytes >>  5) & 0x03u);  // funct2
  instr.funct7 = (uint8_t)((bytes >> 10) & 0x3Fu);  // funct6
  instr.dst    = (uint8_t)((bytes >>  7) & 0x07u);
  instr.src1   = (uint8_t)((bytes >>  7) & 0x07u);
  instr.src2   = (uint8_t)((bytes >>  2) & 0x07u);
  instr.imm.s    = 0;
  instr.size = 2;
  instr.type = OPTYPE_CA;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void InstrRV32IManipulator::executeTypeCA(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.funct7 == 0b00) {
    // c.sub
    regs.gpr[tbl[instr.dst]].val.s = regs.gpr[tbl[instr.src1]].val.s - regs.gpr[tbl[instr.src2]].val.s;
  } else
  if (instr.funct7 == 0b01) {
    // c.xor
    regs.gpr[tbl[instr.dst]].val.s = regs.gpr[tbl[instr.src1]].val.s ^ regs.gpr[tbl[instr.src2]].val.s;
  } else
  if (instr.funct7 == 0b10) {
    // c.or
    regs.gpr[tbl[instr.dst]].val.s = regs.gpr[tbl[instr.src1]].val.s | regs.gpr[tbl[instr.src2]].val.s;
  } else
  if (instr.funct7 == 0b11) {
    // c.and
    regs.gpr[tbl[instr.dst]].val.s = regs.gpr[tbl[instr.src1]].val.s & regs.gpr[tbl[instr.src2]].val.s;
  }
}

void
InstrRV32IManipulator::decodeTypeCB2(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x03u);
  instr.funct3 = (uint8_t)((bytes >> 10) & 0x03u);  // funct2
  instr.funct7 = (uint8_t)((bytes >> 13) & 0x07u);  // funct3
  instr.dst    = (uint8_t)((bytes >>  7) & 0x07u);
  instr.src1   = (uint8_t)((bytes >>  7) & 0x07u);
  instr.src2   = 0;
  instr.imm.s    = sext(8,
      (((bytes >> 12) & 0x01u) <<  5) |
      (((bytes >>  2) & 0x1Fu) <<  0)
      );
  instr.size = 2;
  instr.type = OPTYPE_CB;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void InstrRV32IManipulator::executeTypeCB2(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.funct3 == 0b00) {
    // c.srli
    regs.gpr[tbl[instr.dst]].val.u = regs.gpr[tbl[instr.src1]].val.u >> instr.imm.s;
  } else
  if (instr.funct3 == 0b01) {
    // c.srai
    regs.gpr[tbl[instr.dst]].val.s = regs.gpr[tbl[instr.src1]].val.s >> instr.imm.s;
  } else
  if (instr.funct3 == 0b10) {
    // c.andi
    regs.gpr[tbl[instr.dst]].val.s = regs.gpr[tbl[instr.src1]].val.s & instr.imm.s;
  }
}

void
InstrRV32IManipulator::decodeTypeCB(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x03u);
  instr.funct3 = (uint8_t)((bytes >> 13) & 0x07u);
  instr.dst    = 0;
  instr.src1   = (uint8_t)((bytes >>  7) & 0x07u);
  instr.src2   = 0;
  instr.imm.s    = sext(8,
      (((bytes >> 12) & 0x01u) <<  8) |
      (((bytes >> 10) & 0x03u) <<  3) |
      (((bytes >>  5) & 0x03u) <<  6) |
      (((bytes >>  3) & 0x03u) <<  1) |
      (((bytes >>  2) & 0x01u) <<  5)
      );
  instr.size = 2;
  instr.type = OPTYPE_CB;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void InstrRV32IManipulator::executeTypeCB(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  bool cond = false;
  if (instr.funct3 == 0b110) {
    // c.beqz
    cond = regs.gpr[instr.src1].val.s == 0;
    if (cond) {
      regs.pc.val.u += instr.imm.u;
    }
  } else
  if (instr.funct3 == 0b111) {
    // c.bnez
    cond = regs.gpr[instr.src1].val.s != 0;
    if (cond) {
      regs.pc.val.u += instr.imm.u;
    }
  }
  if (cond) {
    instr.isJumped = true;
  }
}

void
InstrRV32IManipulator::decodeTypeCJ(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x03u);
  instr.funct3 = (uint8_t)((bytes >> 13) & 0x07u);
  instr.dst    = 0;
  instr.src1   = 0;
  instr.src2   = 0;
  instr.imm.s    = sext(11,
      (((bytes >> 12) & 0x01u) << 11) |
      (((bytes >> 11) & 0x01u) <<  4) |
      (((bytes >>  9) & 0x03u) <<  8) |
      (((bytes >>  8) & 0x01u) << 10) |
      (((bytes >>  7) & 0x01u) <<  6) |
      (((bytes >>  6) & 0x01u) <<  7) |
      (((bytes >>  3) & 0x03u) <<  1) |
      (((bytes >>  2) & 0x01u) <<  5)
      );
  instr.size = 2;
  instr.type = OPTYPE_CJ;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void InstrRV32IManipulator::executeTypeCJ(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.funct3 == 0b001) {
    // c.jal
    regs.gpr[1].val.u = regs.pc.val.u + 4;
    regs.pc.val.u = instr.imm.u;
    instr.isJumped = true;
  } else
  if (instr.funct3 == 0b101) {
    // c.j
    regs.pc.val.u = instr.imm.u;
    instr.isJumped = true;
  }
}
