#include <memory>
#include <common.h>
#include <cpu.h>
#include <instruction.h>
#include <memory.h>

#include <fmt/core.h>

bool InstrRV32IManipulator::checkInterruption(Instruction& instr, RegisterSet& regs, Memory& memory)
{
  // check CSR.
  auto mstatus = regs.csr[MSTATUS].val.u;
  auto mie = regs.csr[MIE].val.u;
  // fmt::print("checkInterruption() mstatus={:x} mie={:x}\n", mstatus, mie);
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
  instr.imm.u = regs.csr[MEPC].val.u;  // [DEBUG]
  regs.pc.val.u = regs.csr[MEPC].val.u;
  instr.isJumped = true;
}
