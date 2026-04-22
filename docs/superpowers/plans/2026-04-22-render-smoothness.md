# Render Smoothness Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Eliminate visible flicker and tearing by enforcing a stable frame loop with dirty-region tracking and consistent double-buffer flush discipline.

**Architecture:** All drawing goes to the off-screen buffer at `0xffff0`; `Flush()` copies to VRAM once per frame at the end of `ProcessTasks()`. A dirty-flag prevents unnecessary full-screen copies. Frame pacing uses a simple spin-wait loop tied to PIT tick count.

**Tech Stack:** x86 bare-metal, 32-bit protected mode, VBE 0x111 (640×480 16bpp RGB565), freestanding C, NASM

---

## File Map

- Modify: `boot/main.c` — add frame pacing, call `Flush()` exactly once per frame
- Modify: `boot/graphics.c` — add dirty flag, skip `Flush()` if nothing changed
- Modify: `boot/graphics.h` — expose `MarkDirty()`, `IsDirty()`, frame counter
- Modify: `boot/input.c` — add PIT init for tick-based timing
- Modify: `boot/kernel_entry.asm` — wire PIT ISR (IRQ0 → INT0)

---

### Task 1: Add PIT tick counter for frame pacing

**Files:**
- Modify: `boot/input.c` — add `pit_ticks` global, `InitPIT()`, `HandleISR0`
- Modify: `boot/kernel_entry.asm` — add `isr0` and `LoadIDT` IDT entry for INT0
- Modify: `boot/graphics.h` — declare `extern int pit_ticks`

- [ ] **Step 1: Add PIT globals and ISR to `boot/input.c`**

Add after existing globals (`mx`, `my`, etc.):

```c
int pit_ticks = 0;

void HandleISR0() {
    pit_ticks++;
    // Send EOI to PIC
    __asm__ volatile ("outb %0, %1" : : "a"((unsigned char)0x20), "Nd"((unsigned short)0x20));
}

void InitPIT() {
    // Set PIT channel 0, mode 3 (square wave), ~100 Hz
    // Divisor = 1193180 / 100 = 11931
    unsigned short divisor = 11931;
    __asm__ volatile (
        "outb %0, %1\n"  // command: channel 0, lobyte/hibyte, mode 3
        : : "a"((unsigned char)0x36), "Nd"((unsigned short)0x43)
    );
    __asm__ volatile (
        "outb %0, %1\n"
        : : "a"((unsigned char)(divisor & 0xFF)), "Nd"((unsigned short)0x40)
    );
    __asm__ volatile (
        "outb %0, %1\n"
        : : "a"((unsigned char)((divisor >> 8) & 0xFF)), "Nd"((unsigned short)0x40)
    );
}
```

- [ ] **Step 2: Wire ISR0 in `boot/kernel_entry.asm`**

Add after `isr12:` block:

```nasm
extern HandleISR0
global isr0

isr0:
    pusha
    call HandleISR0
    popa
    iret
```

And in `IDTDesc` / `LoadIDT` — the IDT already has 2048 bytes (256 entries × 8 bytes). The C side sets entries. No change needed in asm beyond exporting `isr0`.

- [ ] **Step 3: Add IDT entry for IRQ0 in `boot/input.c` `InitIDT()`**

Inside `InitIDT()` (already sets entries for INT1 and INT12), add:

```c
extern void isr0();
// IRQ0 → INT0 (PIC remapped: IRQ0 maps to INT0 after remap in RemapPIC)
_idt[0].offset_low  = (unsigned int)isr0 & 0xffff;
_idt[0].selector    = 0x08;
_idt[0].zero        = 0;
_idt[0].type_attr   = 0x8e;
_idt[0].offset_high = ((unsigned int)isr0 >> 16) & 0xffff;
```

Note: `RemapPIC()` currently remaps IRQ1→INT1, IRQ12→INT12. IRQ0 (PIT) goes to INT0 after remap offset 0. Verify `RemapPIC` offsets — if master PIC is remapped to base 0x20, IRQ0 = INT0x20. Adjust IDT index to match. Read `input.c:RemapPIC` before editing.

- [ ] **Step 4: Call `InitPIT()` in `boot/main.c` `start()` before `LoadIDT()`**

```c
InitPIT();
// ... existing mouse init ...
LoadIDT();
```

- [ ] **Step 5: Build and verify no crash**

```bash
make
make run
```

Expected: QEMU boots, OS runs as before. PIT ticking in background (no visible change yet).

- [ ] **Step 6: Commit**

```bash
git add boot/input.c boot/kernel_entry.asm boot/main.c
git commit -m "feat: add PIT tick counter at ~100Hz for frame pacing"
```

---

### Task 2: Enforce single Flush() per frame in main loop

**Files:**
- Modify: `boot/main.c` — remove any stray `Flush()` calls inside tasks, add frame-rate cap

- [ ] **Step 1: Read `boot/main.c` and `boot/task.c` to confirm Flush call locations**

Check: is `Flush()` called anywhere inside task functions or only in `start()`'s loop?

```bash
grep -n "Flush" boot/main.c boot/task.c boot/final.c
```

Expected: only one `Flush()` call in the main `while(1)` loop. If found inside tasks, move them out.

- [ ] **Step 2: Add frame-rate cap to `boot/main.c` main loop**

Replace the main loop:

```c
// Before loop
int last_tick = 0;
int target_ticks_per_frame = 1; // at 100Hz PIT, 1 tick = ~10ms = ~100fps cap

while (1) {
    // Wait for next tick
    while (pit_ticks == last_tick) {
        __asm__ volatile ("hlt");
    }
    last_tick = pit_ticks;

    ProcessTasks();
    Flush();
}
```

`hlt` stops the CPU until next interrupt — eliminates busy-spin that wastes cycles and causes timing jitter.

- [ ] **Step 3: Build and test**

```bash
make && make run
```

Expected: smoother motion for the bouncing ball. No flicker. CPU usage in QEMU noticeably lower.

- [ ] **Step 4: Commit**

```bash
git add boot/main.c
git commit -m "perf: cap frame loop to PIT tick rate, use hlt between frames"
```

---

### Task 3: Add dirty flag to skip redundant Flush() calls

**Files:**
- Modify: `boot/graphics.c` — add `screen_dirty` flag, set in `Draw()`, clear in `Flush()`
- Modify: `boot/graphics.h` — declare `screen_dirty`

- [ ] **Step 1: Add dirty flag to `boot/graphics.c`**

At top of file, after includes:

```c
int screen_dirty = 0;
```

In `Draw()`, add before the pixel write:

```c
screen_dirty = 1;
```

In `Flush()`, wrap copy in dirty check:

```c
void Flush() {
    if (!screen_dirty) return;
    screen_dirty = 0;
    VBEInfoBlock* VBE = (VBEInfoBlock*) VBEInfoAddress;
    unsigned short* buffer = (unsigned short*) ScreenBufferAddress;
    int index;
    for (int y = 0; y < VBE->y_resolution; y++) {
        for (int x = 0; x < VBE->x_resolution; x++) {
            index = y * VBE->x_resolution + x;
            *((unsigned short*)VBE->screen_ptr + index) = *(buffer + index);
        }
    }
}
```

- [ ] **Step 2: Declare in `boot/graphics.h`**

```c
extern int screen_dirty;
```

- [ ] **Step 3: Build and test**

```bash
make && make run
```

Expected: identical visual output. Frames where nothing changes skip the memcpy entirely.

- [ ] **Step 4: Commit**

```bash
git add boot/graphics.c boot/graphics.h
git commit -m "perf: skip Flush() when screen buffer unchanged"
```

---

### Task 4: Fix mouse cursor flicker (erase-draw pattern)

**Files:**
- Modify: `boot/task.c` — `DrawMouseTask` should erase previous cursor before redrawing

The cursor currently draws on top without erasing the previous position, leaving ghost pixels unless `ClearScreenTask` happens to run first.

- [ ] **Step 1: Read `DrawMouseTask` in `boot/task.c`**

Find the task function. It calls `DrawMouse(mx, my, ...)`. Note that `ClearScreen` runs every frame — if `ClearScreenTask` runs before `DrawMouseTask` in priority order, cursor is already erased. Verify task priorities.

```bash
grep -n "DrawMouseTask\|ClearScreenTask\|priority" boot/task.c
```

- [ ] **Step 2: If ClearScreen priority > DrawMouse priority, no change needed**

`ClearScreenTask` should have lower priority number (higher priority = runs first). If it already runs before mouse, the erase is handled. Confirm and document.

If NOT: change `ClearScreenTask` priority to 0 (highest), `DrawMouseTask` to 5 (lowest). In `boot/task.c` `RegisterTask` calls:

```c
RegisterTask(ClearScreenTask, 0);  // priority 0 = first
// ... other tasks ...
RegisterTask(DrawMouseTask, 5);    // priority 5 = last
```

- [ ] **Step 3: Build and test cursor smoothness**

```bash
make && make run
```

Move mouse rapidly. Expected: no ghost pixels, cursor follows pointer exactly with no trail.

- [ ] **Step 4: Commit**

```bash
git add boot/task.c
git commit -m "fix: ensure ClearScreen runs before DrawMouse to eliminate cursor ghosts"
```
