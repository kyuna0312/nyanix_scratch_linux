/* kernel.ld — linker script for Nyanix
 *
 * Memory map:
 *   0x1000        kernel loads here (MBR reads 50 sectors → up to 0x7400)
 *   0x8000        VBEInfoBlock (reserved — must not be touched by linker)
 *   0x90000       stack
 *   0xffff0       off-screen framebuffer
 *
 * We place .text at 0x1000. Remaining sections follow immediately.
 * 50 sectors = 25600 bytes, so code+data must fit below 0x7400.
 * .bss is zero-init at runtime — does not count toward disk image size.
 * We place .bss after 0x90000 (past stack) so it never collides with VBE.
 */

ENTRY(_start)

SECTIONS {
    . = 0x1000;

    .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
        *(.got*)
        *(.got.plt*)
    }

    /* BSS placed well above VBE (0x8000) and stack (0x90000) */
    . = 0xA0000;
    .bss : {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }

    /DISCARD/ : {
        *(.eh_frame*)
        *(.comment*)
        *(.note*)
    }
}
