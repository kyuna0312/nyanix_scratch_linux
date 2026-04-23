// ARM64 Nyanix Kernel - Shell with Commands
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

#define UART0 ((volatile uint32_t*)0x09000000)

void putc(char c) {
    while (UART0[6] & 0x20);
    UART0[0] = c;
}

void puts(const char* s) { while (*s) putc(*s++); }

void puth(uint32_t v) {
    for (int i = 28; i >= 0; i -= 4) {
        int n = (v >> i) & 0xF;
        putc(n < 10 ? '0' + n : 'A' + n - 10);
    }
}

void newline(void) { putc('\n'); }

// Command buffer
static char cmd[64];
static int cmd_pos = 0;

void clear_cmd(void) {
    for (int i = 0; i < 64; i++) cmd[i] = 0;
    cmd_pos = 0;
}

void backspace(void) {
    if (cmd_pos > 0) {
        cmd_pos--;
        putc(8); putc(' '); putc(8);
    }
}

// Shell commands
void cmd_help(void) {
    puts("NYANIX v2 Commands:");
    puts("  help   - Show this help");
    puts("  mmu    - Enable MMU");
    puts("  time   - Show uptime");
    puts("  clear  - Clear screen");
    puts("  reboot - Reboot system");
}

void cmd_mmu(void) {
    puts("MMU: Already configured");
    puts("TCR: 48-bit VA, 4KB granules");
    puts("MAIR: Normal+Device");
}

void cmd_time(void) {
    puts("Time: Using busy-wait loop");
    puts("Timer: Not available without interrupts");
}

void cmd_clear(void) {
    putc(27); putc('['); putc('2'); putc('J');
    putc(27); putc('['); putc('H');
}

void cmd_reboot(void) {
    puts("Rebooting...");
    while (1);
}

void cmd_unknown(void) {
    puts("Unknown. Type 'help' for commands");
}

void run_cmd(void) {
    if (cmd_pos == 0) return;
    
    // Null terminate
    cmd[cmd_pos] = 0;
    newline();
    
    // Simple compare
    if (cmd[0] == 'h' && cmd[1] == 'e' && cmd[2] == 'l' && cmd[3] == 'p' && cmd[4] == 0) {
        cmd_help();
    } else if (cmd[0] == 'm' && cmd[1] == 'm' && cmd[2] == 'u' && cmd[3] == 0) {
        cmd_mmu();
    } else if (cmd[0] == 't' && cmd[1] == 'i' && cmd[2] == 'm' && cmd[3] == 'e' && cmd[4] == 0) {
        cmd_time();
    } else if (cmd[0] == 'c' && cmd[1] == 'l' && cmd[2] == 'e' && cmd[3] == 'a' && cmd[4] == 'r' && cmd[5] == 0) {
        cmd_clear();
    } else if (cmd[0] == 'r' && cmd[1] == 'e' && cmd[2] == 'b' && cmd[3] == 'o' && cmd[4] == 'o' && cmd[5] == 't' && cmd[6] == 0) {
        cmd_reboot();
    } else {
        cmd_unknown();
    }
    
    clear_cmd();
}

// Poll for input
void poll_input(void) {
    if (UART0[4] & 0x10) {  // RX not empty
        char c = UART0[0] & 0xFF;
        
        if (c == '\n' || c == '\r') {
            run_cmd();
        } else if (c == 8 || c == 0x7F) {
            backspace();
        } else if (cmd_pos < 63 && c >= 32 && c < 127) {
            cmd[cmd_pos++] = c;
            putc(c);
        }
    }
}

void kernel_entry(void) {
    puts("ARM64 NYANIX v2\n");
    puts("Shell ready\n");
    puts("Commands: help mmu time clear reboot\n");
    newline();
    
    // Auto-demo help commands
    puts("NYANIX$ help");
    newline();
    cmd_help();
    newline();
    
    puts("NYANIX$ time");
    newline();
    cmd_time();
    newline();
    
    puts("NYANIX$ mmu");
    newline();
    cmd_mmu();
    newline();
    
    puts("Done! Shell ready for input.\n");
    newline();
    
    // Input loop
    while (1) {
        if (UART0[4] & 0x10) {
            char c = UART0[0] & 0xFF;
            
            if (c == '\n' || c == '\r') {
                run_cmd();
            } else if (c == 8 || c == 0x7F) {
                backspace();
            } else if (cmd_pos < 63 && c >= 32 && c < 127) {
                cmd[cmd_pos++] = c;
                putc(c);
            }
        }
    }
}