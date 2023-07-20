  .section .init, "ax"
  .global _start
_start:
  .option norelax
  la gp, __global_pointer$
  la sp, __stack_top
  jal ra, __init_array
  la t0, _int_handler
  csrrw zero, mtvec, t0
  csrrwi zero, mie, 0x008
  li t0, 0x03000100
  li t1, (1<<17)
  sw t1, 0(t0)
  nop
  nop
  call main
