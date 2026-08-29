.section .text.init
.global _start
.type _start, @function

_start:
    la sp, __stack_top

    la t0, __bss_start
    la t1, __bss_end
    beq t0, t1, .Lbss_done

.Lzero_bss:
    sw zero, 0(t0)
    addi t0, t0, 4
    bltu t0, t1, .Lzero_bss

.Lbss_done:
    call main

.Lhalt:
    wfi
    j .Lhalt
