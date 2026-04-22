// ARM64 Nyanix Kernel - v3
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

// Stack at end of .bss section
extern char boot_stack_end[];

// Debug buffer at fixed location
char debug_buf[256];
int dbg_idx = 0;

void dbg_putc(char c) {
    debug_buf[dbg_idx++] = c;
    if (dbg_idx >= 255) dbg_idx = 0;
}

void dbg_puts(const char* s) { while (*s) dbg_putc(*s++); }

void dbg_hex(uint64_t v) {
    for (int i = 28; i >= 0; i -= 4) {
        uint8_t n = (v >> i) & 0xF;
        dbg_putc(n < 10 ? ('0' + n) : ('A' + n - 10));
    }
}

void time_str(char* out, uint32_t ticks) {
    uint32_t secs = ticks / 1000;
    uint32_t mins = secs / 60;
    secs = secs % 60;
    out[0] = '0' + (mins / 10) % 10;
    out[1] = '0' + mins % 10;
    out[2] = ':';
    out[3] = '0' + (secs / 10) % 10;
    out[4] = '0' + secs % 10;
    out[5] = '\0';
}

// Terminal screen
char term_screen[80 * 25];

// Kernel magic at fixed address
volatile uint32_t* magic = (volatile uint32_t*)0x4007FFFC;
#define KERNEL_MAGIC 0xDEADBEEF

// Stack top from linker
extern char _stack_top[];

void kernel_entry(void) {
    // Set up stack!
    __asm__ volatile ("mov sp, %0" : : "r" (_stack_top));
    
    dbg_puts("KERNEL START\n");
    
    *magic = KERNEL_MAGIC;
    
    uint32_t ticks = 0;
    char buf[16];
    
    while (1) {
        if (ticks % 100 == 0) {
            time_str(buf, ticks);
            for (int i = 0; buf[i]; i++)
                term_screen[0 * 80 + 75 + i] = buf[i];
        }
        
        if (ticks % 1000 == 0) {
            dbg_puts("TICK:");
            dbg_hex(ticks);
            dbg_puts("\n");
        }
        
        ticks++;
        volatile uint32_t i;
        for (i = 0; i < 10000; i++);
    }
}