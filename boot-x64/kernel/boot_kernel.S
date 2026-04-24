.globl _start
.code64
.section .text

_start:
    /* Zero BSS */
    leaq __bss_start(%rip), %rdi
    leaq __bss_end(%rip), %rcx
    subq %rdi, %rcx
    xorl %eax, %eax
    rep stosb

    /* VBE info block at 0x8000: phys_base_ptr at offset 40 (uint32) */
    movl 40(%rip + 0x8000 - . - 4), %edi    /* load fb_phys into edi as arg1 */
    /* Actually load from absolute address */
    movabs $0x8028, %rax   /* VBE ModeInfo phys_base_ptr offset = 0x28 = 40 */
    movl (%rax), %edi

    /* Call kernel_main(fb_phys: uint32_t) */
    callq kernel_main

1:  hlt
    jmp 1b

/* ── ISR stubs ─────────────────────────────────────────────────────────── */
.globl isr_pit
isr_pit:
    pushq %rax
    callq handle_pit
    movb $0x20, %al
    outb %al, $0x20
    popq %rax
    iretq

.globl isr_kbd
isr_kbd:
    pushq %rax
    callq handle_kbd
    movb $0x20, %al
    outb %al, $0x20
    popq %rax
    iretq

.globl isr_mouse
isr_mouse:
    pushq %rax
    callq handle_mouse
    movb $0x20, %al
    outb %al, $0xa0
    movb $0x20, %al
    outb %al, $0x20
    popq %rax
    iretq
