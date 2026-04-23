/* ARM64 Kernel Linker */
OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH("aarch64")
ENTRY(_start)

SECTIONS {
    . = 0x40000000;

    .text.boot : { *(.text.boot) } =0
    .text : { *(.text) } =0
    .rodata : { *(.rodata) } =0
    .data : { *(.data) } =0
    .bss : { *(.bss) } =0

    . = ALIGN(16);
    _stack_top = . + 16384;

    /DISCARD/ : { *(.comment) *(.note*) }
}