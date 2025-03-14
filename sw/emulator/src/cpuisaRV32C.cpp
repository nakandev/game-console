#include <memory>
#include <common.h>
#include <cpu.h>
#include <cpuisa.h>
#include <memory.h>

#include <fmt/core.h>

static const uint8_t tbl[8] = {
  8, 9, 10, 11, 12, 13, 14, 15
};

auto CpuIsaRV32I::decode16(uint32_t bytes, Instruction& instr) -> void
{
  uint16_t bytes16 = bytes & 0xFFFFu;
  uint8_t opcode = bytes16 & 0x3u;
  uint8_t funct3 = (bytes16 >> 13) & 0x7u;
  if (opcode == OPCODE_C0 && (funct3 == 0b001 || funct3 == 0b010 || funct3 == 0b011)) {
    decodeTypeCL(bytes16, instr);
  } else
  if (opcode == OPCODE_C0 && (funct3 == 0b101 || funct3 == 0b110 || funct3 == 0b111)) {
    decodeTypeCS(bytes16, instr);
  } else
  if (opcode == OPCODE_C0 && (funct3 == 0b000)) {
    decodeTypeCIW(bytes16, instr);
  } else
  if (opcode == OPCODE_C1 && (funct3 == 0b001 || funct3 == 0b101)) {
    decodeTypeCJ(bytes16, instr);
  } else
  if (opcode == OPCODE_C1 && (funct3 == 0b000 || funct3 == 0b001 || funct3 == 0b010 || funct3 == 0b011)) {
    decodeTypeCI(bytes16, instr);
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
  if (opcode == OPCODE_C2 && (funct3 == 0b000 || funct3 == 0b010)) {
    decodeTypeCI(bytes16, instr);
  } else
  if (opcode == OPCODE_C2 && (funct3 == 0b100)) {
    decodeTypeCR(bytes16, instr);
  } else
  if (opcode == OPCODE_C2 && (funct3 == 0b101 || funct3 == 0b110 || funct3 == 0b111)) {
    decodeTypeCSS(bytes16, instr);
  } else
  {
    instr = Instruction();
    instr.size = 2;
  }
}

auto CpuIsaRV32I::decodeTypeCR(uint32_t bytes, Instruction& instr) -> void
{
  instr.binary = bytes;
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x03u);
  uint8_t funct3 = (uint8_t)((bytes >> 12) & 0x0Fu);  // funct4
  uint8_t funct7 = 0;
  instr.dst    = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src1   = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src2   = (uint8_t)((bytes >>  2) & 0x1Fu);
  instr.imm.s    = 0;
  instr.size = 2;
  instr.type = OPTYPE_CR;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;

  instr.instr = INSTR_UNKNOWN;
  if (funct3 == 0b1000) {
    if (instr.src2 == 0) {
      instr.instr = INSTR_CJR;
    } else
    {
      instr.instr = INSTR_CMV;
    }
  } else
  if (funct3 == 0b1001) {
    if (instr.src2 == 0 && instr.src1 == 0) {
      instr.instr = INSTR_CEBREAK;
    } else
    if (instr.src2 == 0) {
      instr.instr = INSTR_CJALR;
    } else
    if (instr.src1 == 0) {
      // hint
    } else
    {
      instr.instr = INSTR_CADD;
    }
  }
}

auto CpuIsaRV32I::decodeTypeCI(uint32_t bytes, Instruction& instr) -> void
{
  instr.binary = bytes;
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x03u);
  uint8_t funct3 = (uint8_t)((bytes >> 13) & 0x07u);  // funct3
  instr.dst    = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src1   = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src2   = 0;
  instr.imm.s    = sext(5,
      (((bytes >> 12) & 0x01u) <<  5) |
      (((bytes >>  2) & 0x1Fu) <<  0)
      );
  instr.size = 2;
  instr.type = OPTYPE_CI;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;

  instr.instr = INSTR_UNKNOWN;
  if (opcode == OPCODE_C2) {
    if (funct3 == 0b010) {
      instr.instr = INSTR_CLWSP;
      instr.src1   = 2;
      instr.imm.s    = sext(7,
          (((bytes >> 12) & 0x01u) <<  5) |
          (((bytes >>  4) & 0x07u) <<  2) |
          (((bytes >>  2) & 0x03u) <<  6)
          );
    } else
    if (funct3 == 0b000) {
      instr.instr = INSTR_CSLLI;
    } else
    {
      // c.ldsp, c.lqsp, c.flwsp, c.fldsp (rv64/rv128 only)
    }
  } else
  if (opcode == OPCODE_C1) {
    if (funct3 == 0b010) {
      instr.src1   = 0;
      instr.instr = INSTR_CLI;
    } else
    if (funct3 == 0b011) {
      if (instr.dst == 0) {
        // hint
      } else
      if (instr.dst == 2) {
        instr.instr = INSTR_CADDI16SP;
        instr.src1   = 2;
        instr.imm.s    = sext(9,
            (((bytes >> 12) & 0x01u) <<  9) |
            (((bytes >>  6) & 0x01u) <<  4) |
            (((bytes >>  5) & 0x01u) <<  6) |
            (((bytes >>  3) & 0x03u) <<  7) |
            (((bytes >>  2) & 0x01u) <<  5)
            );
      } else
      {
        instr.instr = INSTR_CLUI;
        instr.src1   = 0;
        instr.imm.s    = sext(17,
            (((bytes >> 12) & 0x01u) << 17) |
            (((bytes >>  2) & 0x1Fu) << 12)
            );
      }
    } else
    if (funct3 == 0b000) {
      if (instr.dst != 0) {
        instr.instr = INSTR_CADDI;
      } else
      {
        instr.instr = INSTR_CNOP;
      }
    } else
    if (funct3 == 0b001) {
      // c.addiw (rv64/rv128 only)
    }
  }
}

auto CpuIsaRV32I::decodeTypeCSS(uint32_t bytes, Instruction& instr) -> void
{
  instr.binary = bytes;
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x03u);
  uint8_t funct3 = (uint8_t)((bytes >> 13) & 0x07u);
  instr.dst    = 2;
  instr.src1   = 0;
  instr.src2   = (uint8_t)((bytes >>  2) & 0x1Fu);
  instr.imm.s    = sext(7,
      (((bytes >>  9) & 0x0Fu) <<  2) |
      (((bytes >>  7) & 0x03u) <<  6)
      );
  instr.size = 2;
  instr.type = OPTYPE_CSS;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;

  instr.instr = INSTR_UNKNOWN;
  if (opcode == OPCODE_C2) {
    if (funct3 == 0b110) {
      instr.instr = INSTR_CSWSP;
    } else
    {
      // c.sdsp, c.sqsp, c.fswsp, c.fsdsp (rv64/rv128 only)
    }
  }
}

auto CpuIsaRV32I::decodeTypeCIW(uint32_t bytes, Instruction& instr) -> void
{
  instr.binary = bytes;
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x00u);
  uint8_t funct3 = (uint8_t)((bytes >> 13) & 0x07u);
  instr.dst    = (uint8_t)((bytes >>  2) & 0x07u);
  instr.src1   = 0;
  instr.src2   = 0;
  instr.imm.s    = ((bytes >> 5) & 0xFFu);
  instr.size = 2;
  instr.type = OPTYPE_CIW;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;

  instr.instr = INSTR_CADDI4SPN;
}

auto CpuIsaRV32I::decodeTypeCL(uint32_t bytes, Instruction& instr) -> void
{
  instr.binary = bytes;
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x03u);
  uint8_t funct3 = (uint8_t)((bytes >> 13) & 0x07u);
  instr.dst    = (uint8_t)((bytes >>  2) & 0x07u);
  instr.src1   = (uint8_t)((bytes >>  7) & 0x07u);
  instr.src2   = 0;
  instr.imm.s    = (6,
      (((bytes >> 10) & 0x07u) << 3) |
      (((bytes >>  6) & 0x01u) << 2) |
      (((bytes >>  5) & 0x01u) << 6)
      );
  instr.size = 2;
  instr.type = OPTYPE_CL;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;

  instr.instr = INSTR_UNKNOWN;
  if (funct3 == 0b010) {
    // c.lw
    instr.instr = INSTR_CLW;
  } else
  if (funct3 == 0b011) {
    // c.flw
    // c.ld (rv64/rv128 only)
  } else
  if (funct3 == 0b001) {
    // c.fld (rv64/rv128 only)
    // c.lq (rv64/rv128 only)
  }
}

auto CpuIsaRV32I::decodeTypeCS(uint32_t bytes, Instruction& instr) -> void
{
  instr.binary = bytes;
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x03u);
  uint8_t funct3 = (uint8_t)((bytes >> 13) & 0x07u);
  instr.dst    = 0;
  instr.src1   = (uint8_t)((bytes >>  7) & 0x07u);
  instr.src2   = (uint8_t)((bytes >>  2) & 0x07u);
  instr.imm.s    = (6,
      (((bytes >> 10) & 0x07u) << 3) |
      (((bytes >>  6) & 0x01u) << 2) |
      (((bytes >>  5) & 0x01u) << 6)
      );
  instr.size = 2;
  instr.type = OPTYPE_CS;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;

  instr.instr = INSTR_UNKNOWN;
  if (funct3 == 0b110) {
    instr.instr = INSTR_CSW;
  } else
  if (funct3 == 0b111) {
    // c.fsw
    // c.sd (rv64/rv128 only)
  } else
  if (funct3 == 0b101) {
    // c.fsd (rv64/rv128 only)
    // c.sq (rv64/rv128 only)
  }
}

auto CpuIsaRV32I::decodeTypeCA(uint32_t bytes, Instruction& instr) -> void
{
  instr.binary = bytes;
  instr.opcode = (uint8_t)((bytes >>  0) & 0x03u);
  uint8_t funct2 = (uint8_t)((bytes >>  5) & 0x03u);  // funct2
  uint8_t funct6 = (uint8_t)((bytes >> 10) & 0x3Fu);  // funct6
  instr.dst    = (uint8_t)((bytes >>  7) & 0x07u);
  instr.src1   = (uint8_t)((bytes >>  7) & 0x07u);
  instr.src2   = (uint8_t)((bytes >>  2) & 0x07u);
  instr.imm.s    = 0;
  instr.size = 2;
  instr.type = OPTYPE_CA;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;

  instr.instr = INSTR_UNKNOWN;
  if (funct2 == 0b00) {
    instr.instr = INSTR_CSUB;
  } else
  if (funct2 == 0b01) {
    instr.instr = INSTR_CXOR;
  } else
  if (funct2 == 0b10) {
    instr.instr = INSTR_COR;
  } else
  if (funct2 == 0b11) {
    instr.instr = INSTR_CAND;
  }
}

auto CpuIsaRV32I::decodeTypeCB2(uint32_t bytes, Instruction& instr) -> void
{
  instr.binary = bytes;
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x03u);
  uint8_t funct2 = (uint8_t)((bytes >> 10) & 0x03u);  // funct2
  uint8_t funct3 = (uint8_t)((bytes >> 13) & 0x07u);  // funct3
  instr.dst    = (uint8_t)((bytes >>  7) & 0x07u);
  instr.src1   = (uint8_t)((bytes >>  7) & 0x07u);
  instr.src2   = 0;
  instr.imm.s    = sext(5,
      (((bytes >> 12) & 0x01u) <<  5) |
      (((bytes >>  2) & 0x1Fu) <<  0)
      );
  instr.size = 2;
  instr.type = OPTYPE_CB2;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;

  instr.instr = INSTR_UNKNOWN;
  if (funct2 == 0b00) {
    instr.instr = INSTR_CSRLI;
  } else
  if (funct2 == 0b01) {
    instr.instr = INSTR_CSRAI;
  } else
  if (funct2 == 0b10) {
    instr.instr = INSTR_CANDI;
  }
}

auto CpuIsaRV32I::decodeTypeCB(uint32_t bytes, Instruction& instr) -> void
{
  instr.binary = bytes;
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x03u);
  uint8_t funct3 = (uint8_t)((bytes >> 13) & 0x07u);
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

  instr.instr = INSTR_UNKNOWN;
  if (funct3 == 0b110) {
    instr.instr = INSTR_CBEQZ;
  } else
  if (funct3 == 0b111) {
    instr.instr = INSTR_CBNEZ;
  }
}

auto CpuIsaRV32I::decodeTypeCJ(uint32_t bytes, Instruction& instr) -> void
{
  instr.binary = bytes;
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x03u);
  uint8_t funct3 = (uint8_t)((bytes >> 13) & 0x07u);
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
      (((bytes >>  3) & 0x07u) <<  1) |
      (((bytes >>  2) & 0x01u) <<  5)
      );
  instr.size = 2;
  instr.type = OPTYPE_CJ;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;

  instr.instr = INSTR_UNKNOWN;
  if (funct3 == 0b001) {
    instr.instr = INSTR_CJAL;
  } else
  if (funct3 == 0b101) {
    instr.instr = INSTR_CJ;
  }
}

auto CpuIsaRV32I::execute_cunknown(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
}

// type CR
auto CpuIsaRV32I::execute_cjr(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  if (instr.src1 != 0) {
    regs.pc.val.s = regs.gpr[instr.src1].val.s + 0;
    instr.isJumped = true;
  }
}

auto CpuIsaRV32I::execute_cmv(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  if (instr.dst != 0) {
    regs.gpr[instr.dst].val.s = regs.gpr[instr.src2].val.s;
  }
}

auto CpuIsaRV32I::execute_cebreak(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
}

auto CpuIsaRV32I::execute_cjalr(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  auto tmp = regs.gpr[instr.src1].val.s;
  regs.gpr[1].val.u = regs.pc.val.u + 2;
  regs.pc.val.s = tmp + 0;
  instr.isJumped = true;
}

auto CpuIsaRV32I::execute_cadd(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s + regs.gpr[instr.src2].val.s;
}

// type CI
auto CpuIsaRV32I::execute_clwsp(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  uint32_t addr = regs.gpr[2].val.u + instr.imm.s;
  if (!memory.waitAccess(addr, 4, true, instr.waitCycle)) {
    if (instr.dst != 0) {
      // regs.gpr[instr.dst].val.s = memory.read(addr, 4);
      regs.gpr[instr.dst].val.s = memory.read32(addr);
    }
  }
}

auto CpuIsaRV32I::execute_cslli(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s << instr.imm.s;
}

auto CpuIsaRV32I::execute_cli(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  int32_t imm = instr.imm.s;
  if (instr.dst != 0) {
    regs.gpr[instr.dst].val.s = imm;
  }
}

auto CpuIsaRV32I::execute_caddi16sp(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  int32_t imm = instr.imm.s;
  // regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s + imm;
  regs.gpr[2].val.s = regs.gpr[2].val.s + imm;
}

auto CpuIsaRV32I::execute_clui(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  int32_t imm = instr.imm.s;
  if (imm != 0) {
    regs.gpr[instr.dst].val.s = imm;
  } else
  {
    // error: imm can be non-zero value.
  }
}

auto CpuIsaRV32I::execute_caddi(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  int32_t imm = instr.imm.s;
  int32_t newimm = imm;
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s + newimm;
}

auto CpuIsaRV32I::execute_cnop(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
}

// type CSS
auto CpuIsaRV32I::execute_cswsp(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  uint32_t addr = regs.gpr[2].val.u + instr.imm.s;
  if (!memory.waitAccess(addr, 4, true, instr.waitCycle)) {
    // memory.write(addr, 4, regs.gpr[instr.src2].val.s);
    memory.write32(addr, regs.gpr[instr.src2].val.s);
  }
}

// type CIW
auto CpuIsaRV32I::execute_caddi4spn(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  regs.gpr[tbl[instr.dst]].val.s = regs.gpr[2].val.s + instr.imm.u * 4;
}

// type CL
auto CpuIsaRV32I::execute_clw(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  uint32_t addr = regs.gpr[tbl[instr.src1]].val.u + instr.imm.s;
  if (!memory.waitAccess(addr, 4, true, instr.waitCycle)) {
    // regs.gpr[tbl[instr.dst]].val.s = memory.read(addr, 4);
    regs.gpr[tbl[instr.dst]].val.s = memory.read32(addr);
  }
}

// type CS
auto CpuIsaRV32I::execute_csw(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  uint32_t addr = regs.gpr[tbl[instr.src1]].val.u + instr.imm.s;
  if (!memory.waitAccess(addr, 4, true, instr.waitCycle)) {
    // memory.write(addr, 4, regs.gpr[tbl[instr.src2]].val.s);
    memory.write32(addr, regs.gpr[tbl[instr.src2]].val.s);
  }
}

// type CA
auto CpuIsaRV32I::execute_csub(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  regs.gpr[tbl[instr.dst]].val.s = regs.gpr[tbl[instr.src1]].val.s - regs.gpr[tbl[instr.src2]].val.s;
}

auto CpuIsaRV32I::execute_cxor(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  regs.gpr[tbl[instr.dst]].val.s = regs.gpr[tbl[instr.src1]].val.s ^ regs.gpr[tbl[instr.src2]].val.s;
}

auto CpuIsaRV32I::execute_cor(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  regs.gpr[tbl[instr.dst]].val.s = regs.gpr[tbl[instr.src1]].val.s | regs.gpr[tbl[instr.src2]].val.s;
}

auto CpuIsaRV32I::execute_cand(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  regs.gpr[tbl[instr.dst]].val.s = regs.gpr[tbl[instr.src1]].val.s & regs.gpr[tbl[instr.src2]].val.s;
}

// type CB2
auto CpuIsaRV32I::execute_csrli(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  regs.gpr[tbl[instr.dst]].val.u = regs.gpr[tbl[instr.src1]].val.u >> instr.imm.s;
}

auto CpuIsaRV32I::execute_csrai(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  regs.gpr[tbl[instr.dst]].val.s = regs.gpr[tbl[instr.src1]].val.s >> instr.imm.s;
}

auto CpuIsaRV32I::execute_candi(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  regs.gpr[tbl[instr.dst]].val.s = regs.gpr[tbl[instr.src1]].val.s & instr.imm.s;
}

auto CpuIsaRV32I::execute_cbeqz(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  bool cond = false;
  cond = regs.gpr[tbl[instr.src1]].val.s == 0;
  if (cond) {
    regs.pc.val.u += instr.imm.u;
    instr.isJumped = true;
  }
}

auto CpuIsaRV32I::execute_cbnez(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  bool cond = false;
  cond = regs.gpr[tbl[instr.src1]].val.s != 0;
  if (cond) {
    regs.pc.val.u += instr.imm.u;
    instr.isJumped = true;
  }
}

// type CJ
auto CpuIsaRV32I::execute_cjal(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  regs.gpr[1].val.u = regs.pc.val.u + 2;
  regs.pc.val.u = regs.pc.val.u + instr.imm.s;
  instr.isJumped = true;
}

auto CpuIsaRV32I::execute_cj(Instruction& instr, RegisterSet& regs, Memory& memory) -> void
{
  regs.pc.val.u = regs.pc.val.u + instr.imm.s;
  instr.isJumped = true;
}
