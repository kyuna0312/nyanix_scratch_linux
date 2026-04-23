/* x86_64 Nyanix Kernel - Basic Shell */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

#define VGA_BUFFER ((volatile uint16_t*)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define UART0 ((volatile uint32_t*)0x10000000)

enum { VGA_BLACK = 0, VGA_BLUE = 1, VGA_GREEN = 2, VGA_CYAN = 3, VGA_RED = 4, VGA_MAGENTA = 5, VGA_BROWN = 6, VGA_LIGHT_GREY = 7, VGA_DARK_GREY = 8, VGA_LIGHT_BLUE = 9, VGA_LIGHT_GREEN = 10, VGA_LIGHT_CYAN = 11, VGA_LIGHT_RED = 12, VGA_LIGHT_MAGENTA = 13, VGA_LIGHT_BROWN = 14, VGA_WHITE = 15 };

static int cursor_x = 0;
static int cursor_y = 0;

void putc(char c) {
    while (UART0[6] & 0x20);
    UART0[0] = c;
    if (c == '\n') { cursor_x = 0; cursor_y++; if (cursor_y >= VGA_HEIGHT) cursor_y = 0; return; }
    if (c == '\r') { cursor_x = 0; return; }
    if (c == 8) { if (cursor_x > 0) cursor_x--; return; }
    if (c < 32 || c > 126) return;
    VGA_BUFFER[cursor_y * VGA_WIDTH + cursor_x] = (VGA_WHITE << 8) | c;
    cursor_x++; if (cursor_x >= VGA_WIDTH) { cursor_x = 0; cursor_y++; if (cursor_y >= VGA_HEIGHT) cursor_y = 0; }
}

void puts(const char* s) { while (*s) putc(*s++); }
void puth(uint64_t v) { for (int i = 60; i >= 0; i -= 4) { int n = (v >> i) & 0xF; putc(n < 10 ? '0' + n : 'A' + n - 10); } }
void newline(void) { putc('\n'); putc('\r'); }
void clear_screen(void) { cursor_x = 0; cursor_y = 0; for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) VGA_BUFFER[i] = (VGA_WHITE << 8) | ' '; }

static char cmd[64];
static int cmd_pos = 0;

void clear_cmd(void) { for (int i = 0; i < 64; i++) cmd[i] = 0; cmd_pos = 0; }
void backspace(void) { if (cmd_pos > 0) { cmd_pos--; putc(8); putc(' '); putc(8); } }

void cmd_help(void) { puts("NYANIX x86_64 Commands:"); puts("  help - Show this help"); puts("  clear - Clear screen"); puts("  reboot - Reboot system"); }
void cmd_clear(void) { clear_screen(); }
void cmd_reboot(void) { puts("Rebooting..."); while (1); }

void run_cmd(void) {
    if (cmd_pos == 0) return;
    cmd[cmd_pos] = 0; newline();
    if (cmd[0] == 'h' && cmd[1] == 'e' && cmd[2] == 'l' && cmd[3] == 'p' && cmd[4] == 0) cmd_help();
    else if (cmd[0] == 'c' && cmd[1] == 'l' && cmd[2] == 'e' && cmd[3] == 'a' && cmd[4] == 'r' && cmd[5] == 0) cmd_clear();
    else if (cmd[0] == 'r' && cmd[1] == 'e' && cmd[2] == 'b' && cmd[3] == 'o' && cmd[4] == 'o' && cmd[5] == 't' && cmd[6] == 0) cmd_reboot();
    else puts("Unknown. Try 'help'");
    clear_cmd();
}

void prompt(void) { puts("NYANIX$ "); }

void _start(void) {
    clear_screen();
    puts("NYANIX v3 - x86_64 Bare Metal");
    newline();
    puts("Shell ready");
    newline();
    puts("Commands: help clear reboot");
    newline(); newline();
    prompt();
    while (1) {
        if (UART0[4] & 0x10) {
            char c = UART0[0] & 0xFF;
            if (c == '\n' || c == '\r') { run_cmd(); prompt(); }
            else if (c == 8 || c == 0x7F) backspace();
            else if (cmd_pos < 63 && c >= 32 && c < 127) { cmd[cmd_pos++] = c; putc(c); }
        }
    }
}