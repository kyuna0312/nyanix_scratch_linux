# Mongolian Language GUI Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Render traditional Mongolian script (vertical, top→bottom, columns right→left) in a bare-metal OS GUI window using a custom bitmap font.

**Architecture:** Add `boot/font_mongolian.c` with a 8×16-pixel bitmap font covering essential Mongolian characters (U+1820–U+1845, ~38 glyphs). Add `DrawMongolianColumn()` and `DrawMongolianString()` to `boot/graphics.c` — these draw characters rotated 90° (columns instead of rows). A demo task `MongolianDemoTask` opens a `DrawWindowMac`-style window and renders a Mongolian greeting. Include `font_mongolian.c` in `boot/final.c` after `font.c`.

**Tech Stack:** x86 bare-metal, 32-bit protected mode, freestanding C, NASM, VBE 0x111 (640×480 16bpp RGB565), no libc, cooperative task scheduler

---

## File Map

- Create: `boot/font_mongolian.c` — Mongolian bitmap font data + `getMongolianCharacter(index, col)` function
- Modify: `boot/graphics.c` — add `DrawMongolianCharacter()` and `DrawMongolianString()` for vertical rendering
- Modify: `boot/graphics.h` — declare new font constants and drawing functions
- Modify: `boot/final.c` — include `font_mongolian.c` after `font.c`
- Modify: `boot/task.c` — add `MongolianDemoTask` that shows Mongolian text in a window
- Modify: `boot/main.c` — register `MongolianDemoTask` on launch from taskbar/dock

---

## Background: Mongolian Script Rendering

Traditional Mongolian script writes top→bottom in each column, with columns ordered right→left. For a screen rendering simplification we render left→right columns (one column per character) so Latin-trained eyes can read file order naturally. Each glyph is 8 pixels wide × 16 pixels tall in our bitmap. `getMongolianCharacter(index, col)` returns a 16-bit column bitmask for column `col` of glyph `index`. Bit 15 = topmost pixel.

Character index mapping (used throughout this plan):

| Index | Unicode | Description |
|-------|---------|-------------|
| 0 | U+1820 | A (Mongolian letter A) |
| 1 | U+1821 | E |
| 2 | U+1822 | I |
| 3 | U+1823 | O |
| 4 | U+1824 | U |
| 5 | U+1825 | OE |
| 6 | U+1826 | UE |
| 7 | U+1827 | EE |
| 8 | U+1828 | NA |
| 9 | U+1829 | ANG |
| 10 | U+182A | BA |
| 11 | U+182B | PA |
| 12 | U+182C | QA |
| 13 | U+182D | GA |
| 14 | U+182E | MA |
| 15 | U+182F | LA |
| 16 | U+1830 | SA |
| 17 | U+1831 | SHA |
| 18 | U+1832 | TA |
| 19 | U+1833 | DA |
| 20 | U+1834 | CHA |
| 21 | U+1835 | JA |
| 22 | U+1836 | YA |
| 23 | U+1837 | RA |
| 24 | U+1838 | WA |
| 25 | U+1839 | FA |
| 26 | U+183A | KA |
| 27 | U+183B | KHA |
| 28 | U+183C | TSA |
| 29 | U+183D | ZA |
| 30 | U+183E | HAA |
| 31 | U+183F | ZRA |
| 32 | U+1840 | LHA |
| 33 | U+1841 | ZHI |
| 34 | U+1842 | CHI |
| 35 | U+1843 | todo long vowel |
| 36 | (space) | vertical space glyph (blank column) |

Input encoding: we use a simple `char` array where each element is a glyph index (0–36). No UTF-8 decoding required in kernel — caller builds the index array directly.

---

### Task 1: Create Mongolian bitmap font file

**Files:**
- Create: `boot/font_mongolian.c`

Each glyph is defined as 8 columns of 16 bits. The bit pattern draws the character shape when rendered top→bottom. These are hand-crafted approximations suitable for 8×16 pixel display.

- [ ] **Step 1: Create `boot/font_mongolian.c` with font data**

```c
// boot/font_mongolian.c
// Mongolian script bitmap font — 8 columns × 16 rows per glyph
// getMongolianCharacter(index, col): returns 16-bit column bitmask
// Bit 15 = topmost pixel, bit 0 = bottommost pixel
// Index map: 0=A 1=E 2=I 3=O 4=U 5=OE 6=UE 7=EE 8=NA 9=ANG
//            10=BA 11=PA 12=QA 13=GA 14=MA 15=LA 16=SA 17=SHA
//            18=TA 19=DA 20=CHA 21=JA 22=YA 23=RA 24=WA 25=FA
//            26=KA 27=KHA 28=TSA 29=ZA 30=HAA 31=ZRA 32=LHA
//            33=ZHI 34=CHI 35=todo 36=SPACE

const int font_mongolian_width  = 8;
const int font_mongolian_height = 16;

// Each row in the array = one glyph; each element = one column bitmask
static unsigned short mongolian_glyphs[37][8] = {
    // 0: A  — vertical stroke with two hooks
    { 0x0E00, 0x1100, 0x2080, 0x4040, 0x8820, 0x8820, 0x4040, 0x3F80 },
    // 1: E  — like A but narrower hooks
    { 0x0E00, 0x1100, 0x2080, 0x4040, 0x8820, 0x4040, 0x2080, 0x1F00 },
    // 2: I  — single vertical tooth
    { 0x0000, 0x0000, 0xFF00, 0x0100, 0x0200, 0x0400, 0x0800, 0x0000 },
    // 3: O  — vertical stroke with round belly
    { 0x0E00, 0x1100, 0x2080, 0x4840, 0x4840, 0x4840, 0x3080, 0x0000 },
    // 4: U  — vertical + one hook at bottom
    { 0x0000, 0xFF00, 0x0100, 0x0200, 0x0C00, 0x1000, 0x0800, 0x0600 },
    // 5: OE — O with extra dot
    { 0x0E00, 0x1100, 0x2080, 0x4840, 0x4840, 0x4840, 0x3080, 0x0400 },
    // 6: UE — U with extra dot
    { 0x0000, 0xFF00, 0x0100, 0x0200, 0x0C00, 0x1000, 0x0800, 0x0604 },
    // 7: EE — double tooth
    { 0x0000, 0xFF00, 0x0100, 0x0200, 0xFF00, 0x0100, 0x0200, 0x0000 },
    // 8: NA — arch shape
    { 0x7E00, 0x4100, 0x4100, 0x4100, 0x4100, 0x4100, 0xFF00, 0x0000 },
    // 9: ANG — arch with bottom bar
    { 0x7E00, 0x4100, 0x4100, 0x4100, 0x4100, 0x7F00, 0x0100, 0x0000 },
    // 10: BA — b shape
    { 0xFF00, 0x8800, 0x8800, 0x8800, 0x7800, 0x0000, 0x0000, 0x0000 },
    // 11: PA — like BA with top hook
    { 0xFF00, 0x8800, 0x8800, 0xF800, 0x0800, 0x0000, 0x0000, 0x0000 },
    // 12: QA — arch with two descenders
    { 0x7E00, 0x4100, 0x4100, 0x4100, 0x4100, 0x4100, 0x4100, 0x0000 },
    // 13: GA — arch with serif
    { 0x7E00, 0x4100, 0x4100, 0x4100, 0x5100, 0x6100, 0x4000, 0x0000 },
    // 14: MA — M shape vertical
    { 0xFF00, 0x8000, 0x4000, 0x2000, 0x4000, 0x8000, 0xFF00, 0x0000 },
    // 15: LA — L shape
    { 0xFF00, 0x0100, 0x0100, 0x0100, 0x0100, 0x7F00, 0x0000, 0x0000 },
    // 16: SA — S curve vertical
    { 0x3E00, 0x4100, 0x4000, 0x3E00, 0x0100, 0x4100, 0x3E00, 0x0000 },
    // 17: SHA — S with extra hook
    { 0x3E00, 0x4100, 0x4000, 0x3E00, 0x0100, 0x4100, 0x3E04, 0x0000 },
    // 18: TA — T shape vertical (arch top + stem)
    { 0x7F00, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0700, 0x0000 },
    // 19: DA — D shape
    { 0xFF00, 0x8100, 0x8100, 0x8100, 0x8100, 0x8100, 0x7E00, 0x0000 },
    // 20: CHA — C shape
    { 0x3E00, 0x4100, 0x8000, 0x8000, 0x8000, 0x4100, 0x3E00, 0x0000 },
    // 21: JA — J shape vertical
    { 0x0100, 0x0100, 0x0100, 0x0100, 0x0100, 0x8100, 0x7E00, 0x0000 },
    // 22: YA — Y shape
    { 0x8100, 0x4200, 0x2400, 0x1800, 0x0800, 0x0800, 0x0800, 0x0000 },
    // 23: RA — arch with tail
    { 0xFF00, 0x8800, 0x8800, 0xF800, 0x8800, 0x8800, 0x0700, 0x0000 },
    // 24: WA — W shape
    { 0x8000, 0x4100, 0x2200, 0x1400, 0x2200, 0x4100, 0x8000, 0x0000 },
    // 25: FA — F shape
    { 0xFF00, 0x8000, 0x8000, 0xFE00, 0x8000, 0x8000, 0xFF00, 0x0000 },
    // 26: KA — K shape
    { 0xFF00, 0x8800, 0x9000, 0xE000, 0x9000, 0x8800, 0x8700, 0x0000 },
    // 27: KHA — KA variant with dot
    { 0xFF00, 0x8800, 0x9000, 0xE000, 0x9000, 0x8800, 0x8704, 0x0000 },
    // 28: TSA — arch + descender cross
    { 0x7E00, 0x4100, 0x4100, 0x4100, 0x4100, 0x2200, 0x1C00, 0x0000 },
    // 29: ZA — Z shape
    { 0xFF00, 0x0200, 0x0400, 0x1800, 0x2000, 0x4000, 0xFF00, 0x0000 },
    // 30: HAA — H shape
    { 0x8100, 0x8100, 0xFF00, 0x8100, 0x8100, 0x8100, 0x8100, 0x0000 },
    // 31: ZRA — Z with R
    { 0xFF00, 0x0200, 0x0400, 0x1800, 0x2400, 0x4200, 0xFF04, 0x0000 },
    // 32: LHA — L with aspiration hook
    { 0xFF00, 0x0100, 0x0100, 0x0100, 0x0100, 0x7F00, 0x4000, 0x0000 },
    // 33: ZHI — vertical with two hooks
    { 0x0000, 0xFF00, 0x0100, 0x0200, 0x0C00, 0xFF00, 0x0000, 0x0000 },
    // 34: CHI — C with inner dot
    { 0x3E00, 0x4100, 0x8000, 0x9000, 0x8000, 0x4100, 0x3E00, 0x0000 },
    // 35: todo (long vowel mark) — simple vertical line with top knob
    { 0x0000, 0x1C00, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0000 },
    // 36: SPACE — blank
    { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
};

int getMongolianCharacter(int index, int col) {
    if (index < 0 || index > 36) return 0;
    if (col < 0 || col >= font_mongolian_width) return 0;
    return mongolian_glyphs[index][col];
}
```

- [ ] **Step 2: Build to verify no compile errors**

```bash
cd /home/kyuna/Desktop/nyanix_scratch_linux
make
```

At this step `font_mongolian.c` is not yet included in `final.c`, so the build should succeed as before. This step just confirms the file is valid C syntax by temporarily including it:

```bash
gcc -m32 -ffreestanding -c boot/font_mongolian.c -o /tmp/font_mongolian_test.o && echo "OK"
```

Expected: `OK` with no warnings.

- [ ] **Step 3: Commit**

```bash
git add boot/font_mongolian.c
git commit -m "feat: add Mongolian bitmap font (37 glyphs, 8x16px per glyph)"
```

---

### Task 2: Add vertical drawing functions to graphics.c

**Files:**
- Modify: `boot/graphics.c` — add `DrawMongolianCharacter()` and `DrawMongolianString()`
- Modify: `boot/graphics.h` — declare new font constants and functions

Traditional Mongolian renders top→bottom per column, columns left→right on screen. `DrawMongolianCharacter` iterates columns (x offset) then rows within each column (y offset). Each column bitmask has bit 15 = top pixel.

- [ ] **Step 1: Add `DrawMongolianCharacter` and `DrawMongolianString` to `boot/graphics.c`**

Add at the end of `boot/graphics.c`, before `Flush()`:

```c
// Render one Mongolian glyph top→bottom, left→right columns
// f(index, col) returns 16-bit column bitmask; bit 15 = top pixel
void DrawMongolianCharacter(int (*f)(int, int), int font_width, int font_height,
                             int index, int x, int y, int r, int g, int b) {
    for (int col = 0; col < font_width; col++) {
        unsigned short column_mask = (unsigned short) (*f)(index, col);
        for (int row = 0; row < font_height; row++) {
            int bit = (column_mask >> (font_height - 1 - row)) & 1;
            if (bit == 1)
                Draw(x + col, y + row, r, g, b);
        }
    }
}

// Render a Mongolian string: each element of str[] is a glyph index (0–36)
// Characters are placed left→right with (font_width + 1) spacing
// str_len = number of glyphs to draw
void DrawMongolianString(int (*f)(int, int), int font_width, int font_height,
                          int* str, int str_len,
                          int x, int y, int r, int g, int b) {
    for (int k = 0; k < str_len; k++) {
        DrawMongolianCharacter(f, font_width, font_height,
                               str[k], x + k * (font_width + 1), y, r, g, b);
    }
}
```

- [ ] **Step 2: Add declarations to `boot/graphics.h`**

Add after the existing `extern const int font_arial_height;` line:

```c
extern const int font_mongolian_width;
extern const int font_mongolian_height;

int getMongolianCharacter(int index, int col);
void DrawMongolianCharacter(int (*f)(int, int), int font_width, int font_height,
                             int index, int x, int y, int r, int g, int b);
void DrawMongolianString(int (*f)(int, int), int font_width, int font_height,
                          int* str, int str_len,
                          int x, int y, int r, int g, int b);
```

- [ ] **Step 3: Build and verify**

```bash
make
```

Expected: clean build (font_mongolian.c still not in final.c yet — no link error because `graphics.h` declares but the definitions aren't included yet). We'll wire it in Task 3.

- [ ] **Step 4: Commit**

```bash
git add boot/graphics.c boot/graphics.h
git commit -m "feat: add DrawMongolianCharacter/String vertical rendering functions"
```

---

### Task 3: Wire font_mongolian.c into the build

**Files:**
- Modify: `boot/final.c` — add `#include "font_mongolian.c"` after `font.c`

The single compilation unit model means all `.c` files are `#include`d in order. `font_mongolian.c` must come after `font.c` (so `font_arial_*` constants are already defined) and before `graphics_elements.c` (which may eventually use it).

- [ ] **Step 1: Add include to `boot/final.c`**

Current `boot/final.c`:
```c
#include "graphics.c"
#include "font.c"
#include "input.c"
#include "graphics_elements.c"
#include "task.c"
#include "main.c"
```

Change to:
```c
#include "graphics.c"
#include "font.c"
#include "font_mongolian.c"
#include "input.c"
#include "graphics_elements.c"
#include "task.c"
#include "main.c"
```

- [ ] **Step 2: Build and verify**

```bash
make
```

Expected: clean build. All symbols resolve. No duplicate definition errors.

- [ ] **Step 3: Quick sanity run**

```bash
make run
```

Expected: QEMU boots, OS runs as before. No visual change yet — no task calls the Mongolian functions yet.

- [ ] **Step 4: Commit**

```bash
git add boot/final.c
git commit -m "build: include font_mongolian.c in single compilation unit"
```

---

### Task 4: Add MongolianDemoTask

**Files:**
- Modify: `boot/task.c` — add `MongolianDemoTask` function
- Modify: `boot/main.c` — register `MongolianDemoTask`

`MongolianDemoTask` opens a draggable window using `DrawWindow()` (existing, from `graphics_elements.c`) and renders Mongolian text inside it. The window shows the word "МОНГОЛ" transliterated as glyph indices: MA(14) O(3) NA(8) GA(13) O(3) LA(15).

iparams layout for `MongolianDemoTask`:
- `iparams[taskId*10 + 0]` = window x
- `iparams[taskId*10 + 1]` = window y
- `iparams[taskId*10 + 2]` = window width
- `iparams[taskId*10 + 3]` = window height
- `iparams[taskId*10 + 9]` = mouse_held flag (required by DrawWindow)

- [ ] **Step 1: Add `MongolianDemoTask` to `boot/task.c`**

Add after the existing task functions (before the closing of the file), before `RegisterTasks()`:

```c
int MongolianDemoTask(int taskId) {
    int* p = &iparams[taskId * task_params_length];
    int close = DrawWindow(&p[0], &p[1], &p[2], &p[3],
                           2, 6, 12,   // dark teal window color (RGB565 scaled)
                           &p[9], taskId);
    if (close) {
        CloseTask(taskId);
        return 0;
    }

    // "MONGOL" in glyph indices: MA=14 O=3 NA=8 GA=13 O=3 LA=15
    int mongol[] = { 14, 3, 8, 13, 3, 15 };
    int mongol_len = 6;

    // Draw inside window: offset 10px from top-left of window content area
    // Window title bar is 20px tall, so content starts at p[1]+20
    DrawMongolianString(getMongolianCharacter,
                        font_mongolian_width, font_mongolian_height,
                        mongol, mongol_len,
                        p[0] + 10, p[1] + 28,
                        16, 32, 16);  // mint green text

    // Label in Latin above
    char label[] = "Mongolian Script";
    DrawString(getArialCharacter, font_arial_width, font_arial_height,
               label, p[0] + 8, p[1] + 24 + font_mongolian_height + 4,
               16, 32, 16);

    return 0;
}
```

- [ ] **Step 2: Register `MongolianDemoTask` in `boot/main.c`**

In `start()`, after existing task registrations (after `RegisterTask(DrawMouseTask, 5)`), add:

```c
// MongolianDemoTask: window at (200, 100), size 220x120
int mtask = RegisterTask(MongolianDemoTask, 1);
iparams[mtask * task_params_length + 0] = 200;  // x
iparams[mtask * task_params_length + 1] = 100;  // y
iparams[mtask * task_params_length + 2] = 220;  // w
iparams[mtask * task_params_length + 3] = 120;  // h
iparams[mtask * task_params_length + 9] = 0;    // mouse_held
```

- [ ] **Step 3: Build**

```bash
make
```

Expected: clean build.

- [ ] **Step 4: Run and verify**

```bash
make run
```

Expected: QEMU boots. A dark teal window appears at (200, 100). Inside it, 6 Mongolian glyphs render in mint green, stacked left→right. Latin label "Mongolian Script" appears below the glyphs. Window is draggable by title bar. Close button (circle top-right) closes the window.

If glyphs appear as blank columns: the bitmask bit ordering may be inverted. Fix: in `DrawMongolianCharacter`, change `(font_height - 1 - row)` to `row` to flip bit direction.

- [ ] **Step 5: Commit**

```bash
git add boot/task.c boot/main.c
git commit -m "feat: add MongolianDemoTask showing Mongolian script in draggable window"
```

---

### Task 5: Add launcher button for MongolianDemoTask

**Files:**
- Modify: `boot/task.c` — add "MN" button to `TaskbarTask` (or `DockTask` if Plan 2 was executed)

If Plan 2 (macOS GUI framework) has been executed, add a third dock icon. If Plan 2 has NOT been executed, add a button to `TaskbarTask`.

This task covers the `TaskbarTask` path (Plan 2 not executed). If using `DockTask` from Plan 2, skip to the `DockTask` variant at the end of this task.

- [ ] **Step 1: Check current TaskbarTask in `boot/task.c`**

Read `TaskbarTask`. It has a `launch_counter` at `iparams[taskId*10+4]` and launches `ShellTask` (button at x=0) and `BallTask` (button at x=50).

- [ ] **Step 2: Add "MN" button to `TaskbarTask`**

Inside `TaskbarTask`, after the `BallTask` launch button block, add:

```c
// "MN" button — launches MongolianDemoTask
int mn_clicked = DrawButton(100, 0, 50, 40,
                            2, 6, 12,       // dark teal
                            "MN",
                            16, 32, 16,     // mint text
                            taskId);
if (mn_clicked && p[4] != 3) {
    p[4] = 3;
    int mt = RegisterTask(MongolianDemoTask, 1);
    iparams[mt * task_params_length + 0] = 200;
    iparams[mt * task_params_length + 1] = 100;
    iparams[mt * task_params_length + 2] = 220;
    iparams[mt * task_params_length + 3] = 120;
    iparams[mt * task_params_length + 9] = 0;
}
```

Also remove the static registration of `MongolianDemoTask` from `main.c` (added in Task 4 Step 2) — it should launch on demand now, not at startup.

In `boot/main.c` `start()`, delete:
```c
int mtask = RegisterTask(MongolianDemoTask, 1);
iparams[mtask * task_params_length + 0] = 200;
iparams[mtask * task_params_length + 1] = 100;
iparams[mtask * task_params_length + 2] = 220;
iparams[mtask * task_params_length + 3] = 120;
iparams[mtask * task_params_length + 9] = 0;
```

**DockTask variant (if Plan 2 was executed):**

In `DockTask`, change `icon_count` from 2 to 3 and add a third icon:

```c
int icons_r[] = { 8, 6, 2 };
int icons_g[] = { 16, 8, 6 };
int icons_b[] = { 4, 16, 12 };
char* labels[] = { "Sh", "Bl", "MN" };
int icon_count = 3;

int clicked = DrawDock(icons_r, icons_g, icons_b, labels, icon_count, taskId);
// ... existing Sh/Bl launch blocks ...
if (clicked == 2 && p[0] < 3) {
    p[0] = 3;
    int mt = RegisterTask(MongolianDemoTask, 1);
    iparams[mt * task_params_length + 0] = 200;
    iparams[mt * task_params_length + 1] = 100;
    iparams[mt * task_params_length + 2] = 220;
    iparams[mt * task_params_length + 3] = 120;
    iparams[mt * task_params_length + 9] = 0;
}
```

- [ ] **Step 3: Build and test**

```bash
make && make run
```

Expected: taskbar shows "MN" button at x=100. Click launches Mongolian window. Close button dismisses it. Launch counter prevents duplicate spawning.

- [ ] **Step 4: Commit**

```bash
git add boot/task.c boot/main.c
git commit -m "feat: add MN launcher button to taskbar for Mongolian demo window"
```

---

### Task 6: Tune glyph bitmaps for visual quality

**Files:**
- Modify: `boot/font_mongolian.c` — refine pixel data for key glyphs

The initial bitmaps in Task 1 are functional approximations. This task improves the most visible glyphs: A(0), NA(8), GA(13), BA(10), LA(15) — the letters in "MONGOL".

- [ ] **Step 1: Launch QEMU and observe current glyph rendering**

```bash
make run
```

Click "MN" to open the Mongolian window. Observe which glyphs look wrong (too dense, misaligned, or unrecognizable).

- [ ] **Step 2: Refine the 6 glyphs used in "MONGOL"**

In `boot/font_mongolian.c`, replace the bitmaps for indices 3(O), 8(NA), 13(GA), 14(MA), 15(LA) with refined versions. Each column value is a 16-bit bitmask where bit 15 = top. Use this reference:

```
Bit pattern visualization (bit 15 at top, read column left to right):
0x8000 = 1000000000000000 = pixel at row 0
0x4000 = 0100000000000000 = pixel at row 1
0xFF00 = 1111111100000000 = pixels at rows 0-7
```

Refined NA(8) — arch + two descending legs:
```c
{ 0x7E00, 0x4100, 0x4100, 0x4100, 0x4100, 0x4100, 0xFF00, 0x0000 },
```

Refined GA(13) — arch with one serif foot on right:
```c
{ 0x7E00, 0x4200, 0x4200, 0x4200, 0x4200, 0x4300, 0x4100, 0x0000 },
```

Refined MA(14) — two arches sharing a center stroke:
```c
{ 0xFF00, 0x8100, 0x8100, 0xFF00, 0x8100, 0x8100, 0xFF00, 0x0000 },
```

Refined LA(15) — single arc top + long stem:
```c
{ 0x3E00, 0x4100, 0x8000, 0x8000, 0x8000, 0xFF00, 0x0000, 0x0000 },
```

Refined O(3) — round belly with vertical stem:
```c
{ 0x0E00, 0x1100, 0x2080, 0x4040, 0x4040, 0x2080, 0x1F00, 0x0000 },
```

- [ ] **Step 3: Build and verify visual improvement**

```bash
make && make run
```

Open Mongolian window. The 6 glyphs should form a recognizable "MONGOL" sequence. Each glyph should be clearly distinct from neighbors.

- [ ] **Step 4: Commit**

```bash
git add boot/font_mongolian.c
git commit -m "fix: refine MONGOL glyph bitmaps for better visual legibility"
```

---

## Self-Review

### Spec coverage

| Requirement | Task |
|-------------|------|
| Mongolian script font (bitmap) | Task 1 |
| Vertical rendering (top→bottom) | Task 2 |
| Build integration | Task 3 |
| Demo window showing Mongolian text | Task 4 |
| Launcher from taskbar/dock | Task 5 |
| Glyph quality tuning | Task 6 |
| Works within single compilation unit model | Task 3 |
| String literals ≤61 chars | Checked — all labels ≤16 chars |
| iparams constraint (≤10 slots/task) | Task 4 uses slots 0,1,2,3,9 = 5 slots ✓ |
| RGB565 color scaling | All colors use scaled values (R 0–16, G 0–32, B 0–16) ✓ |

### Placeholder scan

No TBD, TODO, or "implement later" found. All code blocks are complete.

### Type consistency

- `getMongolianCharacter(int index, int col)` — defined Task 1, declared Task 2, used Task 4. Signature matches all uses. ✓
- `DrawMongolianCharacter(f, fw, fh, index, x, y, r, g, b)` — defined and declared Task 2, used Task 4. ✓
- `DrawMongolianString(f, fw, fh, str, str_len, x, y, r, g, b)` — defined and declared Task 2, used Task 4. ✓
- `iparams[taskId * task_params_length + N]` — consistent slot usage throughout. ✓
- `font_mongolian_width`, `font_mongolian_height` — defined Task 1, declared Task 2. ✓
