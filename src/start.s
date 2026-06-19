# start snippet init 
.section .text.start
  .global _start 
_start:
  csrci mstatus,8
  
# end snippet init 

# start snippet stackhi 
  .option push 
  .option norelax
  # li sp, 0x88000000
  la t0, adresa_steka 
  ld sp, 0(t0)
  la gp, __global_pointer$
  .option pop 
# end snippet stackhi 

# start snippet bss 
  la t0, _bss_start 
  la t1, _bss_end 
bss_clear:
    bge t0, t1, bss_done 
    sd zero, 0(t0)
    addi t0, t0, 8 
    j bss_clear 

# end snippet bss 

bss_done: 
  tail monitor
loop:
  j loop

# start snippet stacklo 

.align 8 
adresa_steka:
  .quad _stack_vrh 

# end snippet stacklo 
