.globl _start
_start:
    movz x0, 0x4000
    movk x0, 4, lsl #16
    mov sp, x0
    b kernel_entry