#include <memory>
#include <common.h>
#include <cpu.h>
#include <instruction.h>
#include <memory.h>

#include <fmt/core.h>

void
InstrRV32IManipulator::decodeTypeSystem(uint32_t bytes, Instruction& instr)
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
  instr.type = OPTYPE_SYSTEM;
  instr.result.s = 0;
  instr.isJumped = false;
  instr.isWaiting = false;
}
void
InstrRV32IManipulator::executeTypeSystem(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  if (instr.funct3 == 0b000) {
    // ebraek, ecall, mret, wfi
    if (instr.imm.u == FUNCT12_ECALL) {
      instr.isJumped = true;
    } else
    if (instr.imm.u == FUNCT12_EBREAK) {
      instr.isWaiting = true;
    } else
    if (instr.imm.u == FUNCT12_MRET) {
      // instr.isJumped = true;
      returnInterruption(instr, regs);
    } else
    if (instr.imm.u == FUNCT12_WFI) {
      instr.isWaiting = true;
    } else
    {
    }
  } else
  if (instr.funct3 == 0b001) {
    // csrrw
    union32_t tmp = regs.gpr[instr.src1].val;
    if (instr.dst != 0) {
      regs.gpr[instr.dst].val.s = regs.csr[instr.imm.u].val.s;
    }
    regs.csr[instr.imm.u].val.s = tmp.s;
  } else
  if (instr.funct3 == 0b010) {
    // csrrs
    union32_t tmp = regs.gpr[instr.src1].val;
    if (instr.dst != 0) {
      regs.gpr[instr.dst].val.s = regs.csr[instr.imm.u].val.s;
    }
    if (instr.src1 != 0) {
      regs.csr[instr.imm.u].val.u |= tmp.u;
    }
  } else
  if (instr.funct3 == 0b011) {
    // csrrc
    union32_t tmp = regs.gpr[instr.src1].val;
    if (instr.dst != 0) {
      regs.gpr[instr.dst].val.s = regs.csr[instr.imm.u].val.s;
    }
    if (instr.src1 != 0) {
      regs.csr[instr.imm.u].val.u &= ~tmp.u;
    }
  } else
  if (instr.funct3 == 0b100) {
    // Hypervisor ops
  } else
  if (instr.funct3 == 0b101) {
    // csrrwi
    if (instr.dst != 0) {
      regs.gpr[instr.dst].val.s = regs.csr[instr.imm.u].val.s;
    }
    regs.csr[instr.imm.u].val.s = instr.src1;
  } else
  if (instr.funct3 == 0b110) {
    // csrrsi
    if (instr.dst != 0) {
      regs.gpr[instr.dst].val.s = regs.csr[instr.imm.u].val.s;
    }
    if (instr.src1 != 0) {
      regs.csr[instr.imm.u].val.s |= instr.src1;
    }
  } else
  if (instr.funct3 == 0b111) {
    // csrrci
    if (instr.dst != 0) {
      regs.gpr[instr.dst].val.s = regs.csr[instr.imm.u].val.s;
    }
    if (instr.src1 != 0) {
      regs.csr[instr.imm.u].val.s &= ~instr.src1;
    }
  }

  // doCsrSideEffects();
}

bool InstrRV32IManipulator::checkInterruption(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  // check CSR.
  auto mstatus = regs.csr[MSTATUS].val.u;
  auto mie = regs.csr[MIE].val.u;
  // [TODO] check IORAM if MEIE bit is enable in MIE.
  if ((mstatus & 0x8) && mie) return true;  // dummy conditional..
  return false;
}
void InstrRV32IManipulator::handleInterruption(Instruction& instr, RegisterSet& regs)
{
  // save current PC to MEPC.
  regs.csr[MEPC].val.u = regs.pc.val.u;
  // save current mode to MPP bits in MSTATUS register.
  // - nothing to do on this system.
  // copy MIE bit to MPIE bit in MSTATUS register.
  auto mstatus = regs.csr[MSTATUS].val.u;
  mstatus = (mstatus & 0xffffff0fu) | ((mstatus & 0xfu) << 4);
  // clear MIE bit to Disable Interrupts.
  mstatus = (mstatus & 0xfffffff7u);
  regs.csr[MSTATUS].val.u = mstatus;
  // jump to handler specified in MTVEC register.
  regs.pc.val.u = regs.csr[MTVEC].val.u;
  instr.isWaiting = false;
  instr.isJumped = true;
}

void InstrRV32IManipulator::returnInterruption(Instruction& instr, RegisterSet& regs)
{
  // restore MPIE bit to MIE bit in MSTATUS register.
  auto mstatus = regs.csr[MSTATUS].val.u;
  mstatus = (mstatus & 0xffffff0fu) | ((mstatus & 0xfu) << 4);
  // set current mode from MPP bits in MSTATUS register.
  // - nothing to do on this system.
  // restore MEPC to PC.
  regs.pc.val.u = regs.csr[MEPC].val.u;
  instr.isJumped = true;
}
