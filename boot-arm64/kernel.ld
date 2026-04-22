/* ARM64 Kernel Linker Script */
OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH("aarch64")
ENTRY(_start)

SECTIONS {
    . = 0x40000000;

    .text : {
        *(.text.boot)
        *(.text)
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    . = . + 16384;   /* Reserve stack space */
    _stack_top = .;

    /DISCARD/ : {
        *(.comment)
        *(.note*)
    }
}