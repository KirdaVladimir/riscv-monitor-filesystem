OUTPUT_FORMAT("elf64-littleriscv")
ENTRY(_start)

MEMORY {
  ram (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}
__min_stack_size = 64K; 

_stack_vrh = ABSOLUTE(ALIGN(ORIGIN(ram) + LENGTH(ram), 16)); 

SECTIONS
{
  .text : {
    *(.text.start) 
    *(.text .text.*)
  } > ram  
  .rodata : {
    *(.rodata .rodata.*)  
  } > ram 

  .data : ALIGN(8) {
    _data_start = .; 
    *(.data .data.*) 

    . = ALIGN(8); 
    __global_pointer$ = . + 0x800; 

    *(.sdata .sdata.*)
    _data_end = .; 
  } > ram 
  .bss : {
    _bss_start = .; 
    *(.sbss .sbss.*)
    *(.bss .bss.*)
    *(COMMON)
    _bss_end = .; 
  } > ram
}

ASSERT(_bss_end + __min_stack_size <= _stack_vrh, "Nema dovoljno prostora za stek.")