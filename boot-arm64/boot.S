.section .text.boot
.globl _start
.globl start

_start:
start:
    // Output 'X' immediately
    movz x0, 0x9000, lsl 16
    mov w1, 0x58
    str w1, [x0, 0]
    
    // Set stack
    ldr x0, =boot_stack_end
    mov sp, x0
    
    // Output 'S'
    mov w1, 0x53
    str w1, [x0, 0]
    
    b kernel_entry

.loop:
    b .loop

// Put stack in .data (will be in kernel image)
.section .data
.align 16
.globl boot_stack
boot_stack:
    .skip 16384
.globl boot_stack_end
boot_stack_end: