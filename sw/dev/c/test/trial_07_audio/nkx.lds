/* refs
 * /home/nyalry/nakan/work/study/riscv/gcc/build/riscv64-unknown-elf/lib/ldscripts/elf32lriscv.x
 */
OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv",
	      "elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)
SEARCH_DIR("/home/nyalry/nakan/work/study/riscv/gcc/build/riscv64-unknown-elf/lib");

MEMORY
{
  SYSTEM  (rx)  : ORIGIN = 0x00000000, LENGTH = 0x01000000
  DATARAM (!rx) : ORIGIN = 0x01000000, LENGTH = 0x02000000
  IORAM   (!rx) : ORIGIN = 0x03000000, LENGTH = 0x01000000
  VRAM    (!rx) : ORIGIN = 0x04000000, LENGTH = 0x02000000
  TILERAM (!rx) : ORIGIN = 0x06000000, LENGTH = 0x01000000
  ARAM    (!rx) : ORIGIN = 0x07000000, LENGTH = 0x01000000
  PROGRAM (rx)  : ORIGIN = 0x80000000, LENGTH = 0x04000000
}

SECTIONS
{
  .init           :
  {
    KEEP (*(SORT_NONE(.init)))
  } >PROGRAM
  .text           :
  {
    *(.text.unlikely .text.*_unlikely .text.unlikely.*)
    *(.text.exit .text.exit.*)
    *(.text.startup .text.startup.*)
    *(.text.hot .text.hot.*)
    *(SORT(.text.sorted.*))
    *(.text .stub .text.* .gnu.linkonce.t.*)
    /* .gnu.warning sections are handled specially by elf.em.  */
    *(.gnu.warning)
  } >PROGRAM
  .fini           :
  {
    KEEP (*(SORT_NONE(.fini)))
  } >PROGRAM
  PROVIDE (__etext = .);
  PROVIDE (_etext = .);
  PROVIDE (etext = .);
  .rodata         :
  {
    *(.rodata .rodata.* .gnu.linkonce.r.*)
  } >PROGRAM

  /* . = DATA_SEGMENT_ALIGN (CONSTANT (MAXPAGESIZE), CONSTANT (COMMONPAGESIZE)); */
  .preinit_array    :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >PROGRAM
  .init_array    :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >PROGRAM
  .fini_array    :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >PROGRAM
  .ctors          :
  {
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    /* We don't want to include the .ctor section from
       the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
  } >PROGRAM
  .dtors          :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
  } >PROGRAM

  /* . = DATA_SEGMENT_RELRO_END (0, .); */
  .data           :
  {
    __DATA_BEGIN__ = .;
    *(.data .data.* .gnu.linkonce.d.*)
    SORT(CONSTRUCTORS)
  } >DATARAM
  .sdata          :
  {
    __SDATA_BEGIN__ = .;
    *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata .srodata.*)
    *(.sdata .sdata.* .gnu.linkonce.s.*)
  } >DATARAM
  _edata = .; PROVIDE (edata = .);
  . = .;
  __bss_start = .;
  .sbss           :
  {
    *(.dynsbss)
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
  } >DATARAM
  .bss            :
  {
   *(.dynbss)
   *(.bss .bss.* .gnu.linkonce.b.*)
   *(COMMON)
   /* Align here to ensure that the .bss section occupies space up to
      _end.  Align after .bss to ensure correct alignment even if the
      .bss section disappears because there are no input sections.
      FIXME: Why do we need it? When there is no .bss section, we do not
      pad the .data section.  */
   . = ALIGN(. != 0 ? 32 / 8 : 1);
  } >DATARAM
  . = ALIGN(32 / 8);
  . = SEGMENT_START("ldata-segment", .);
  . = ALIGN(32 / 8);
  __BSS_END__ = .;
    __global_pointer$ = MIN(__SDATA_BEGIN__ + 0x800,
		            MAX(__DATA_BEGIN__ + 0x800, __BSS_END__ - 0x800));
  _end = .; PROVIDE (end = .);
  /* . = DATA_SEGMENT_END (.); */

  PROVIDE (__stack_top = ORIGIN(DATARAM) + LENGTH(DATARAM));

  /* .ioram :      */
  /* {             */
  /*   *(.ioram)   */
  /* } >IORAM      */

  /* .vram :       */
  /* {             */
  /*   *(.vram)    */
  /* } >VRAM       */

  /* .tileram :    */
  /* {             */
  /*   *(.tileram) */
  /* } >TILERAM    */

  /* .aram :       */
  /* {             */
  /*   *(.aram)    */
  /* } >ARAM       */
}

