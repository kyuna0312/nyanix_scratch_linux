#include "../../kernel/graphics.h"

/* ── globals ────────────────────────────────────────────────────────────── */
int mx = 512, my = 384;
int left_clicked = FALSE, right_clicked = FALSE;
volatile int pit_ticks = 0;
int mouse_possessed_task_id = 0;
int Scancode = -1;

/* ── port I/O ──────────────────────────────────────────────────────────── */
static inline unsigned char inb(unsigned short port) {
    unsigned char v;
    __asm__ __volatile__("inb %1, %0" : "=a"(v) : "dN"(port));
    return v;
}
static inline void outb(unsigned short port, unsigned char v) {
    __asm__ __volatile__("outb %1, %0" : : "dN"(port), "a"(v));
}

/* ── 64-bit IDT ─────────────────────────────────────────────────────────── */
struct IDTEntry64 {
    unsigned short off0;
    unsigned short sel;
    unsigned char  ist;
    unsigned char  flags;
    unsigned short off1;
    unsigned int   off2;
    unsigned int   zero;
} __attribute__((packed));

struct IDTDesc {
    unsigned short limit;
    unsigned long long base;
} __attribute__((packed));

static struct IDTEntry64 idt[256];
static struct IDTDesc idt_desc;

extern void isr_pit(void);
extern void isr_kbd(void);
extern void isr_mouse(void);

static void idt_set(int n, void (*handler)(void)) {
    unsigned long long addr = (unsigned long long)handler;
    idt[n].off0  = addr & 0xFFFF;
    idt[n].sel   = 0x18;        /* 64-bit code selector */
    idt[n].ist   = 0;
    idt[n].flags = 0x8E;        /* present, DPL=0, interrupt gate */
    idt[n].off1  = (addr >> 16) & 0xFFFF;
    idt[n].off2  = (addr >> 32) & 0xFFFFFFFF;
    idt[n].zero  = 0;
}

/* ── PIC ────────────────────────────────────────────────────────────────── */
static void pic_remap(void) {
    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28); /* IRQ0-7 → INT 0x20, IRQ8-15 → INT 0x28 */
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    /* unmask IRQ0 (PIT), IRQ1 (KBD), IRQ12 (mouse) */
    outb(0x21, ~((1 << 0) | (1 << 1) | (1 << 4)));
    outb(0xA1, ~(1 << 4));
}

/* ── PS/2 mouse ─────────────────────────────────────────────────────────── */
static void mouse_write(unsigned char b) {
    int t = 100000;
    while (t-- && (inb(0x64) & 2));
    outb(0x64, 0xD4);
    t = 100000;
    while (t-- && (inb(0x64) & 2));
    outb(0x60, b);
}
static unsigned char mouse_read(void) {
    int t = 100000;
    while (t-- && !(inb(0x64) & 1));
    return inb(0x60);
}
static void mouse_init(void) {
    int t = 100000;
    while (t-- && (inb(0x64) & 2));
    outb(0x64, 0xA8);           /* enable aux device */
    t = 100000;
    while (t-- && (inb(0x64) & 2));
    outb(0x64, 0x20);
    t = 100000;
    while (t-- && !(inb(0x64) & 1));
    unsigned char status = inb(0x60) | 2;
    t = 100000;
    while (t-- && (inb(0x64) & 2));
    outb(0x64, 0x60);
    t = 100000;
    while (t-- && (inb(0x64) & 2));
    outb(0x60, status);
    mouse_write(0xFF); mouse_read();
    mouse_write(0xF6); mouse_read();
    mouse_write(0xF4); mouse_read();
}

/* ── ISR handlers ───────────────────────────────────────────────────────── */
static int mouse_byte = 0;
static unsigned char mouse_buf[3];

void handle_pit(void) {
    pit_ticks++;
}

void handle_kbd(void) {
    Scancode = inb(0x60);
}

void handle_mouse(void) {
    unsigned char b = mouse_read();
    if (mouse_byte == 0 && !(b & 0x08)) return;
    mouse_buf[mouse_byte++] = b;
    if (mouse_byte < 3) return;
    mouse_byte = 0;

    unsigned char status = mouse_buf[0];
    int dx = (int)mouse_buf[1];
    int dy = (int)mouse_buf[2];

    if (status & 0x40 || status & 0x80) return;  /* overflow */
    if (status & 0x10) dx |= 0xFFFFFF00;
    if (status & 0x20) dy |= 0xFFFFFF00;

    left_clicked  = (status & 0x01) ? TRUE : FALSE;
    right_clicked = (status & 0x02) ? TRUE : FALSE;

    if (dx > 0) mx += 2; else if (dx < 0) mx -= 2;
    if (dy > 0) my -= 2; else if (dy < 0) my += 2;

    if (mx < 0)    mx = 0;
    if (mx > 1024) mx = 1024;
    if (my < 0)    my = 0;
    if (my > 768)  my = 768;
}

/* ── PIT ────────────────────────────────────────────────────────────────── */
static void pit_init(void) {
    unsigned short div = 11931;
    outb(0x43, 0x36);
    outb(0x40, div & 0xFF);
    outb(0x40, (div >> 8) & 0xFF);
}

/* ── public init ────────────────────────────────────────────────────────── */
void input_init(void) {
    idt_set(0x20, isr_pit);
    idt_set(0x21, isr_kbd);
    idt_set(0x2C, isr_mouse);

    pic_remap();
    pit_init();
    mouse_init();

    idt_desc.limit = sizeof(idt) - 1;
    idt_desc.base  = (unsigned long long)idt;
    __asm__ __volatile__("lidt %0" : : "m"(idt_desc));
    __asm__ __volatile__("sti");
}
