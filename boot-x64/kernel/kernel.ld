/* x86_64 Nyanix Kernel Linker Script */
ENTRY(_start)

SECTIONS {
    . = 0x10000;

    .text : {
        *(.text*)
    } =0x90

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    } =0x00

    .bss : {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }

    /DISCARD/ : {
        *(.eh_frame*)
        *(.comment*)
    }
}