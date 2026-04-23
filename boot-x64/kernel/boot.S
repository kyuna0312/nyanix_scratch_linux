.globl _start
.section .text
_start:
    # Set up stack (grows down)
    movabs $0x200000, %rax  # 2MB stack
    mov %rsp, %rax
    
    # Zero BSS
    movabs $__bss_start, %rdi
    movabs $__bss_end, %rcx
    sub %rdi, %rcx
    xor %rax, %rax
    rep stosb
    
    # Jump to C kernel
    call kernel_entry
    
    # Halt if returned
    1: hlt
    jmp 1b