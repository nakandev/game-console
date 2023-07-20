  .global _int_handler
  .extern int_handler
_int_handler:
  /* Save only Caller-Saved Regs, bucause Callee-Saved Regs will be saved by function. */
  addi sp, sp, -64
  sw ra,  0(sp);
  sw t0,  4(sp);
  sw t1,  8(sp);
  sw t2, 12(sp);
  sw a0, 16(sp);
  sw a1, 20(sp);
  sw a2, 24(sp);
  sw a3, 28(sp);
  sw a4, 32(sp);
  sw a5, 36(sp);
  sw a6, 40(sp);
  sw a7, 44(sp);
  sw t3, 48(sp);
  sw t4, 52(sp);
  sw t5, 56(sp);
  sw t6, 60(sp);
  nop
  call int_handler
  nop
  lw ra,  0(sp);
  lw t0,  4(sp);
  lw t1,  8(sp);
  lw t2, 12(sp);
  lw a0, 16(sp);
  lw a1, 20(sp);
  lw a2, 24(sp);
  lw a3, 28(sp);
  lw a4, 32(sp);
  lw a5, 36(sp);
  lw a6, 40(sp);
  lw a7, 44(sp);
  lw t3, 48(sp);
  lw t4, 52(sp);
  lw t5, 56(sp);
  lw t6, 60(sp);
  addi sp, sp, 64
  mret
