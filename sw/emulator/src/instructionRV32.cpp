#include <memory>
#include <common.h>
#include <cpu.h>
#include <instruction.h>
#include <memory.h>

#include <fmt/core.h>

InstrRV32IManipulator::InstrRV32IManipulator()
{
  initI();
  initC();
}

void
InstrRV32IManipulator::initI()
{
  int i = 0;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_unknown;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_lui;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_auipc;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_jal;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_jalr;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_beq;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_bne;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_blt;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_bge;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_bltu;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_bgeu;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_lb;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_lh;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_lw;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_lbu;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_lhu;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_sb;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_sh;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_sw;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_addi;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_slti;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_sltiu;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_xori;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_ori;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_andi;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_slli;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_srli;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_srai;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_add;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_sub;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_sll;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_slt;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_sltu;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_xor;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_srl;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_sra;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_or;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_and;
  // execute_tableI[i++] = &InstrRV32IManipulator::execute_fence;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_ecall;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_ebreak;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_mret;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_wfi;
  // execute_tableI[i++] = &InstrRV32IManipulator::execute_fencei;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_csrrw;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_csrrs;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_csrrc;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_csrrwi;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_csrrsi;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_csrrci;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_mul;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_mulh;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_mulhsu;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_mulhu;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_div;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_divu;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_rem;
  execute_tableI[i++] = &InstrRV32IManipulator::execute_remu;
}
void
InstrRV32IManipulator::initC()
{
  int i = 0;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cunknown;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_caddi4spn;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_cfld;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_clq;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_clw;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_cflw;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_cld;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_cfsd;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_csq;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_csw;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_cfsw;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_csd;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cnop;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_caddi;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cjal;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_caddiw;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cli;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_caddi16sp;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_clui;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_csrli;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_csrli64;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_csrai;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_csrai64;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_candi;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_csub;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cxor;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cor;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cand;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_csubw;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_caddw;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cj;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cbeqz;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cbnez;

  execute_tableC[i++] = &InstrRV32IManipulator::execute_cslli;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_cslli64;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_cfldsp;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_clqsp;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_clwsp;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_cflwsp;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_cldsp;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cjr;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cmv;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cebreak;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cjalr;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cadd;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_cfsdsp;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_csqsp;
  execute_tableC[i++] = &InstrRV32IManipulator::execute_cswsp;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_cfswsp;
  // execute_tableC[i++] = &InstrRV32IManipulator::execute_csdsp;
}

InstrRV32IManipulator::~InstrRV32IManipulator()
{
}

void
InstrRV32IManipulator::fetch(Instruction& instr)
{
  instr.phase = INSTR_PHASE_DECODE;
}

void
InstrRV32IManipulator::decode(uint32_t bytes, Instruction& instr)
{
  instr.waitCycle = 1;
  if ((bytes & 0x3) != 3) {
    // 16-bits instruction
    decode16(bytes, instr);
  } else 
  if (((bytes >> 2) & 0x7) != 7) {
    // 32-bits instruction
    decode32(bytes, instr);
  } else
  {
    instr = Instruction();
    instr.size = 4;
  }
  instr.phase = INSTR_PHASE_EXECUTE;
}

void
InstrRV32IManipulator::decode32(uint32_t bytes, Instruction& instr)
{
  uint8_t opcode = bytes & 0x7Fu;
  if (opcode == OPCODE_LOAD || opcode == OPCODE_STORE) {
    decodeTypeS(bytes, instr);
  } else
  if (opcode == OPCODE_ArithI || opcode == OPCODE_JALR) {
    decodeTypeI(bytes, instr);
  } else
  if (opcode == OPCODE_Arith) {
    decodeTypeR(bytes, instr);
  } else
  if (opcode == OPCODE_BRANCH) {
    decodeTypeB(bytes, instr);
  } else
  if (opcode == OPCODE_LUI || opcode == OPCODE_AUIPC) {
    decodeTypeU(bytes, instr);
  } else
  if (opcode == OPCODE_JAL) {
    decodeTypeJ(bytes, instr);
  } else
  if (opcode == OPCODE_SYSTEM) {
    decodeTypeSystem(bytes, instr);
  } else
  {
    instr = Instruction();
    instr.size = 4;
  }
}

void
InstrRV32IManipulator::decodeTypeR(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x7Fu);
  uint8_t funct3 = (uint8_t)((bytes >> 12) & 0x07u);
  uint8_t funct7 = (uint8_t)((bytes >> 25) & 0x7Fu);
  instr.dst    = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src1   = (uint8_t)((bytes >> 15) & 0x1Fu);
  instr.src2   = (uint8_t)((bytes >> 20) & 0x1Fu);
  instr.imm.s    = 0;
  instr.size = 4;
  instr.type = OPTYPE_R;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;

  const int instr_table_funct3_0[] = {
    INSTR_ADD,
    INSTR_SLL,
    INSTR_SLT,
    INSTR_SLTU,
    INSTR_XOR,
    INSTR_SRL,
    INSTR_OR,
    INSTR_AND,
  };
  const int instr_table_funct3_1[] = {
    INSTR_MUL,
    INSTR_MULH,
    INSTR_MULHSU,
    INSTR_MULHU,
    INSTR_DIV,
    INSTR_DIVU,
    INSTR_REM,
    INSTR_REMU,
  };
  const int instr_table_funct3_32[] = {
    INSTR_SUB,
    INSTR_SRA,
    INSTR_UNKNOWN,
    INSTR_UNKNOWN,
    INSTR_UNKNOWN,
    INSTR_UNKNOWN,
    INSTR_UNKNOWN,
    INSTR_UNKNOWN,
  };

  instr.instr = INSTR_UNKNOWN;
  if (funct7 == 0) {
    instr.instr = instr_table_funct3_0[funct3];
  } else
  if (funct7 == 1) {
    instr.instr = instr_table_funct3_1[funct3];
    if (funct3 >= INSTR_DIV) {
      instr.waitCycle = 3;
    }
  } else
  if (funct7 == 32) {
    instr.instr = instr_table_funct3_32[funct3];
  }
}

void
InstrRV32IManipulator::decodeTypeI(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x7Fu);
  uint8_t funct3 = (uint8_t)((bytes >> 12) & 0x07u);
  uint8_t funct7 = 0;
  instr.dst    = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src1   = (uint8_t)((bytes >> 15) & 0x1Fu);
  instr.src2   = 0;
  instr.imm.s    = sext(11, (bytes >> 20) & 0xFFFu);
  instr.size = 4;
  instr.type = OPTYPE_I;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;

  const int instr_table_funct3_ArithI[] = {
    INSTR_ADDI,
    INSTR_SLLI,
    INSTR_SLTI,
    INSTR_SLTIU,
    INSTR_XORI,
    INSTR_SRLI,
    INSTR_ORI,
    INSTR_ANDI,
  };

  instr.instr = INSTR_UNKNOWN;
  if (opcode == OPCODE_ArithI) {
    instr.instr = instr_table_funct3_ArithI[funct3];
    if (funct3 == FUNCT3_SRLI) {
      int8_t mode = instr.imm.s >> 5;
      if (mode == 0b0100000) {
        instr.instr = INSTR_SRAI;
      }
    }
  } else
  if (opcode == OPCODE_JALR) {
    if (funct3 == FUNCT3_JALR) {
      instr.instr = INSTR_JALR;
    }
  } else
  if (opcode == OPCODE_SYSTEM) {
    if (funct3 == FUNCT3_PRIV) {
      if (instr.imm.s == FUNCT12_ECALL) {
        instr.instr = INSTR_ECALL;
      } else
      if (instr.imm.s == FUNCT12_EBREAK) {
        instr.instr = INSTR_EBREAK;
      }
    }
  }
}

void
InstrRV32IManipulator::decodeTypeS(uint32_t bytes, Instruction& instr)
{
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x7Fu);
  uint8_t funct3 = (uint8_t)((bytes >> 12) & 0x07u);
  uint8_t funct7 = 0;
  if (opcode == OPCODE_LOAD) {
    instr.binary = bytes;
    opcode = (uint8_t)((bytes >>  0) & 0x7Fu);
    funct3 = (uint8_t)((bytes >> 12) & 0x07u);
    funct7 = 0;
    instr.dst    = (uint8_t)((bytes >>  7) & 0x1Fu);
    instr.src1   = (uint8_t)((bytes >> 15) & 0x1Fu);
    instr.src2   = 0;
    instr.imm.s    = sext(11, ((bytes >> 20) & 0xFFFu));
    instr.size = 4;
    instr.type = OPTYPE_S;
    instr.result.s = 0;
    instr.isJumped = false;
    instr.isWaiting = false;
    instr.waitCycle = -1;
  }
  else {
    instr.binary = bytes;
    opcode = (uint8_t)((bytes >>  0) & 0x7Fu);
    funct3 = (uint8_t)((bytes >> 12) & 0x07u);
    funct7 = 0;
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
    instr.waitCycle = -1;
  }

  const int instr_table_load[] = {
    INSTR_LB,
    INSTR_LH,
    INSTR_LW,
    INSTR_UNKNOWN,
    INSTR_LBU,
    INSTR_LHU,
    INSTR_UNKNOWN,
    INSTR_UNKNOWN,
  };
  const int instr_table_store[] = {
    INSTR_SB,
    INSTR_SH,
    INSTR_SW,
    INSTR_UNKNOWN,
    INSTR_UNKNOWN,
    INSTR_UNKNOWN,
    INSTR_UNKNOWN,
    INSTR_UNKNOWN,
  };

  instr.instr = INSTR_UNKNOWN;
  if (opcode == OPCODE_LOAD) {
    instr.instr = instr_table_load[funct3];
  } else
  if (opcode == OPCODE_STORE) {
    instr.instr = instr_table_store[funct3];
  }
}

void
InstrRV32IManipulator::decodeTypeB(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x7Fu);
  uint8_t funct3 = (uint8_t)((bytes >> 12) & 0x07u);
  uint8_t funct7 = 0;
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

  const int instr_table_funct3[] = {
    INSTR_BEQ,
    INSTR_BNE,
    INSTR_UNKNOWN,
    INSTR_UNKNOWN,
    INSTR_BLT,
    INSTR_BGE,
    INSTR_BLTU,
    INSTR_BGEU,
  };

  instr.instr = instr_table_funct3[funct3];
}

void
InstrRV32IManipulator::decodeTypeU(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x7Fu);
  uint8_t funct3 = 0;
  uint8_t funct7 = 0;
  instr.dst    = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src1   = 0;
  instr.src2   = 0;
  instr.imm.u    = bytes & 0xFFFFF000u;
  instr.size = 4;
  instr.type = OPTYPE_U;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;

  instr.instr = INSTR_UNKNOWN;
  if (opcode == OPCODE_LUI) {
    instr.instr = INSTR_LUI;
  } else
  if (opcode == OPCODE_AUIPC) {
    instr.instr = INSTR_AUIPC;
  }
}

void
InstrRV32IManipulator::decodeTypeJ(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x7Fu);
  uint8_t funct3 = 0;
  uint8_t funct7 = 0;
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

  instr.instr = INSTR_JAL;
}

void
InstrRV32IManipulator::decodeTypeSystem(uint32_t bytes, Instruction& instr)
{
  instr.binary = bytes;
  uint8_t opcode = (uint8_t)((bytes >>  0) & 0x7Fu);
  uint8_t funct3 = (uint8_t)((bytes >> 12) & 0x07u);
  uint8_t funct7 = 0;
  instr.dst    = (uint8_t)((bytes >>  7) & 0x1Fu);
  instr.src1   = (uint8_t)((bytes >> 15) & 0x1Fu);
  instr.src2   = 0;
  instr.imm.s    = sext(11, (bytes >> 20) & 0xFFFu);
  instr.size = 4;
  instr.type = OPTYPE_SYSTEM;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;

  const int instr_table_funct3_csr[] = {
    INSTR_UNKNOWN,
    INSTR_CSRRW,
    INSTR_CSRRS,
    INSTR_CSRRC,
    INSTR_UNKNOWN,
    INSTR_CSRRWI,
    INSTR_CSRRSI,
    INSTR_CSRRCI,
  };

  instr.instr = INSTR_UNKNOWN;
  if (funct3 == 0) {
    if (instr.imm.u == 0x000) {
      instr.instr = INSTR_ECALL;
    } else
    if (instr.imm.u == 0x001) {
      instr.instr = INSTR_EBREAK;
    } else
    if (instr.imm.u == 0x302) {
      instr.instr = INSTR_MRET;
    } else
    if (instr.imm.u == 0x103) {
      instr.instr = INSTR_WFI;
    }
  } else {
    instr.instr = instr_table_funct3_csr[funct3];
  }
}

void
InstrRV32IManipulator::execute(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.size == 2) {
    (this->*(execute_tableC[instr.instr]))(instr, regs, memory);
  } else
  if (instr.size == 4) {
    (this->*(execute_tableI[instr.instr]))(instr, regs, memory);
  }

  instr.waitCycle--;
  if (instr.waitCycle > 0) {
    return;
  }

  if (!instr.isJumped) {
    regs.pc.val.u += instr.size;
  }
  instr.phase = INSTR_PHASE_POSTPROC;
}

void
InstrRV32IManipulator::execute_unknown(Instruction& instr, RegisterSet& regs, Memory& memory)
{
}
// type R
void
InstrRV32IManipulator::execute_add(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s + regs.gpr[instr.src2].val.s;
}
void
InstrRV32IManipulator::execute_sll(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.u = regs.gpr[instr.src1].val.u << regs.gpr[instr.src2].val.u;
}
void
InstrRV32IManipulator::execute_slt(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s < regs.gpr[instr.src2].val.s;
}
void
InstrRV32IManipulator::execute_sltu(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.u = regs.gpr[instr.src1].val.u < regs.gpr[instr.src2].val.u;
}
void
InstrRV32IManipulator::execute_xor(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s ^ regs.gpr[instr.src2].val.s;
}
void
InstrRV32IManipulator::execute_srl(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.u = regs.gpr[instr.src1].val.u >> regs.gpr[instr.src2].val.u;
}
void
InstrRV32IManipulator::execute_or(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s | regs.gpr[instr.src2].val.s;
}
void
InstrRV32IManipulator::execute_and(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s & regs.gpr[instr.src2].val.s;
}
void
InstrRV32IManipulator::execute_mul(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.waitCycle == 1) {
    regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s * regs.gpr[instr.src2].val.s;
  }
}
void
InstrRV32IManipulator::execute_mulh(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.waitCycle == 1) {
    int64_t val = (int64_t)regs.gpr[instr.src1].val.s * regs.gpr[instr.src2].val.s;
    regs.gpr[instr.dst].val.s = (int32_t)(val >> 32);
  }
}
void
InstrRV32IManipulator::execute_mulhsu(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.waitCycle == 1) {
    int64_t val = (int64_t)regs.gpr[instr.src1].val.s * regs.gpr[instr.src2].val.u;
    regs.gpr[instr.dst].val.s = (int32_t)(val >> 32);
  }
}
void
InstrRV32IManipulator::execute_mulhu(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.waitCycle == 1) {
    uint64_t val = (uint64_t)regs.gpr[instr.src1].val.u * regs.gpr[instr.src2].val.u;
    regs.gpr[instr.dst].val.s = (uint32_t)(val >> 32);
  }
}
void
InstrRV32IManipulator::execute_div(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.waitCycle == 1) {
    regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s / regs.gpr[instr.src2].val.s;
  }
}
void
InstrRV32IManipulator::execute_divu(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.waitCycle == 1) {
    regs.gpr[instr.dst].val.u = regs.gpr[instr.src1].val.u / regs.gpr[instr.src2].val.u;
  }
}
void
InstrRV32IManipulator::execute_rem(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.waitCycle == 1) {
    regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s % regs.gpr[instr.src2].val.s;
  }
}
void
InstrRV32IManipulator::execute_remu(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.waitCycle == 1) {
    regs.gpr[instr.dst].val.u = regs.gpr[instr.src1].val.u % regs.gpr[instr.src2].val.u;
  }
}
void
InstrRV32IManipulator::execute_sub(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s - regs.gpr[instr.src2].val.s;
}
void
InstrRV32IManipulator::execute_sra(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s >> regs.gpr[instr.src2].val.s;
}
// type I
void
InstrRV32IManipulator::execute_addi(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s + instr.imm.s;
}
void
InstrRV32IManipulator::execute_slti(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s < instr.imm.s;
}
void
InstrRV32IManipulator::execute_sltiu(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.u = regs.gpr[instr.src1].val.u < instr.imm.u;
}
void
InstrRV32IManipulator::execute_xori(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s ^ instr.imm.s;
}
void
InstrRV32IManipulator::execute_ori(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s | instr.imm.s;
}
void
InstrRV32IManipulator::execute_andi(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s & instr.imm.s;
}
void
InstrRV32IManipulator::execute_slli(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  int8_t imm = instr.imm.s & 0x1F;
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s << imm;
}
void
InstrRV32IManipulator::execute_srli(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  int8_t imm = instr.imm.s & 0x1F;
  regs.gpr[instr.dst].val.u = regs.gpr[instr.src1].val.u >> imm;
}
void
InstrRV32IManipulator::execute_srai(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  int8_t imm = instr.imm.s & 0x1F;
  regs.gpr[instr.dst].val.s = regs.gpr[instr.src1].val.s >> imm;
}
void
InstrRV32IManipulator::execute_jalr(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  int32_t tmp = regs.gpr[instr.src1].val.s;
  if (instr.dst != 0) {
    regs.gpr[instr.dst].val.u = regs.pc.val.u + 4;
  }
  regs.pc.val.s = tmp + instr.imm.s;
  instr.isJumped = true;
}
// type S
void
InstrRV32IManipulator::execute_lb(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  uint32_t addr = regs.gpr[instr.src1].val.u + instr.imm.s;
  if (!memory.waitAccess(addr, 1, true, instr.waitCycle)) {
    regs.gpr[instr.dst].val.s = memory.read(addr, 1);
  }
}
void
InstrRV32IManipulator::execute_lh(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  uint32_t addr = regs.gpr[instr.src1].val.u + instr.imm.s;
  if (!memory.waitAccess(addr, 2, true, instr.waitCycle)) {
    regs.gpr[instr.dst].val.s = memory.read(addr, 2);
  }
}
void
InstrRV32IManipulator::execute_lw(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  uint32_t addr = regs.gpr[instr.src1].val.u + instr.imm.s;
  if (!memory.waitAccess(addr, 4, true, instr.waitCycle)) {
    regs.gpr[instr.dst].val.s = memory.read(addr, 4);
  }
}
void
InstrRV32IManipulator::execute_lbu(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  uint32_t addr = regs.gpr[instr.src1].val.u + instr.imm.s;
  if (!memory.waitAccess(addr, 1, true, instr.waitCycle)) {
    regs.gpr[instr.dst].val.u = memory.read(addr, 1) & 0xffu;
  }
}
void
InstrRV32IManipulator::execute_lhu(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  uint32_t addr = regs.gpr[instr.src1].val.u + instr.imm.s;
  if (!memory.waitAccess(addr, 2, true, instr.waitCycle)) {
    regs.gpr[instr.dst].val.u = memory.read(addr, 2) & 0xffffu;
  }
}
void
InstrRV32IManipulator::execute_sb(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  uint32_t addr = regs.gpr[instr.src1].val.u + instr.imm.s;
  if (!memory.waitAccess(addr, 1, true, instr.waitCycle)) {
    memory.write(addr, 1, regs.gpr[instr.src2].val.s);
  }
}
void
InstrRV32IManipulator::execute_sh(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  uint32_t addr = regs.gpr[instr.src1].val.u + instr.imm.s;
  if (!memory.waitAccess(addr, 2, true, instr.waitCycle)) {
    memory.write(addr, 2, regs.gpr[instr.src2].val.s);
  }
}
void
InstrRV32IManipulator::execute_sw(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  uint32_t addr = regs.gpr[instr.src1].val.u + instr.imm.s;
  if (!memory.waitAccess(addr, 4, true, instr.waitCycle)) {
    memory.write(addr, 4, regs.gpr[instr.src2].val.s);
  }
}
// type B
void
InstrRV32IManipulator::execute_beq(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  bool cond = false;
  cond = regs.gpr[instr.src1].val.s == regs.gpr[instr.src2].val.s;
  if (cond) {
    regs.pc.val.u += instr.imm.u;
    instr.isJumped = true;
  }
}
void
InstrRV32IManipulator::execute_bne(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  bool cond = false;
  cond = regs.gpr[instr.src1].val.s != regs.gpr[instr.src2].val.s;
  if (cond) {
    regs.pc.val.u += instr.imm.u;
    instr.isJumped = true;
  }
}
void
InstrRV32IManipulator::execute_blt(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  bool cond = false;
  cond = regs.gpr[instr.src1].val.s < regs.gpr[instr.src2].val.s;
  if (cond) {
    regs.pc.val.u += instr.imm.u;
    instr.isJumped = true;
  }
}
void
InstrRV32IManipulator::execute_bge(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  bool cond = false;
  cond = regs.gpr[instr.src1].val.s >= regs.gpr[instr.src2].val.s;
  if (cond) {
    regs.pc.val.u += instr.imm.u;
    instr.isJumped = true;
  }
}
void
InstrRV32IManipulator::execute_bltu(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  bool cond = false;
  cond = regs.gpr[instr.src1].val.u < regs.gpr[instr.src2].val.u;
  if (cond) {
    regs.pc.val.u += instr.imm.u;
    instr.isJumped = true;
  }
}
void
InstrRV32IManipulator::execute_bgeu(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  bool cond = false;
  cond = regs.gpr[instr.src1].val.u >= regs.gpr[instr.src2].val.u;
  if (cond) {
    regs.pc.val.u += instr.imm.u;
    instr.isJumped = true;
  }
}
// type U
void
InstrRV32IManipulator::execute_lui(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.dst != 0)
    regs.gpr[instr.dst].val.u = instr.imm.u;
}
void
InstrRV32IManipulator::execute_auipc(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.dst != 0)
    regs.gpr[instr.dst].val.u = regs.pc.val.u + instr.imm.u;
}
// type J
void
InstrRV32IManipulator::execute_jal(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.dst != 0)
    regs.gpr[instr.dst].val.u = regs.pc.val.u + 4;
  regs.pc.val.u += instr.imm.u;
  instr.isJumped = true;
}
// type System
void
InstrRV32IManipulator::execute_ecall(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  instr.isJumped = true;
}
void
InstrRV32IManipulator::execute_ebreak(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  setException(instr, regs, EXCEPTION_BREAKPOINT);
  instr.isWaiting = true;
}
void
InstrRV32IManipulator::execute_mret(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  returnInterruption(instr, regs);
}
void
InstrRV32IManipulator::execute_wfi(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  instr.isWaiting = true;
}
void
InstrRV32IManipulator::execute_csrrw(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  union32_t tmp = regs.gpr[instr.src1].val;
  if (instr.dst != 0) {
    regs.gpr[instr.dst].val.s = regs.csr[instr.imm.u].val.s;
  }
  regs.csr[instr.imm.u].val.s = tmp.s;
}
void
InstrRV32IManipulator::execute_csrrs(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  union32_t tmp = regs.gpr[instr.src1].val;
  if (instr.dst != 0) {
    regs.gpr[instr.dst].val.s = regs.csr[instr.imm.u].val.s;
  }
  if (instr.src1 != 0) {
    regs.csr[instr.imm.u].val.u |= tmp.u;
  }
}
void
InstrRV32IManipulator::execute_csrrc(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  union32_t tmp = regs.gpr[instr.src1].val;
  if (instr.dst != 0) {
    regs.gpr[instr.dst].val.s = regs.csr[instr.imm.u].val.s;
  }
  if (instr.src1 != 0) {
    regs.csr[instr.imm.u].val.u &= ~tmp.u;
  }
}
void
InstrRV32IManipulator::execute_csrrwi(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.dst != 0) {
    regs.gpr[instr.dst].val.s = regs.csr[instr.imm.u].val.s;
  }
  regs.csr[instr.imm.u].val.s = instr.src1;
}
void
InstrRV32IManipulator::execute_csrrsi(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.dst != 0) {
    regs.gpr[instr.dst].val.s = regs.csr[instr.imm.u].val.s;
  }
  if (instr.src1 != 0) {
    regs.csr[instr.imm.u].val.s |= instr.src1;
  }
}
void
InstrRV32IManipulator::execute_csrrci(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.dst != 0) {
    regs.gpr[instr.dst].val.s = regs.csr[instr.imm.u].val.s;
  }
  if (instr.src1 != 0) {
    regs.csr[instr.imm.u].val.s &= ~instr.src1;
  }
}
