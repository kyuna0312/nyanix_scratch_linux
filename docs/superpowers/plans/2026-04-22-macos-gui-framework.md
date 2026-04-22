# macOS-Style GUI Framework Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add macOS-style system menu bar (top), dock (bottom), traffic-light window buttons, and expanded widget library (text input, checkbox, scrollable list) to the existing cooperative task OS.

**Architecture:** Paint order = z-order (priority 0 draws first = bottom). MenuBarTask at priority 0, DockTask at priority 0, windows at priority 1–3, mouse at priority 5. Traffic-light buttons replace single close circle: red/yellow/green at window top-left. New widgets added to `boot/graphics_elements.c`. All state in `iparams` (10 slots/task).

**Tech Stack:** x86 bare-metal, 32-bit protected mode, VBE 0x111 (640×480 16bpp RGB565), freestanding C, NASM

---

## File Map

- Modify: `boot/graphics_elements.c` — add `DrawMenuBar()`, `DrawDock()`, `DrawWindowMac()`, `DrawTextInput()`, `DrawCheckbox()`
- Modify: `boot/graphics.h` — declare new element functions
- Modify: `boot/task.c` — add `MenuBarTask`, `DockTask`, update `TaskbarTask` → dock style, adjust window task priorities
- Modify: `boot/main.c` — register `MenuBarTask` (priority 0), `DockTask` (priority 0), set `DrawMouseTask` priority 5

---

### Task 1: System menu bar (top strip)

**Files:**
- Modify: `boot/graphics_elements.c` — add `DrawMenuBar()`
- Modify: `boot/graphics.h` — declare `DrawMenuBar()`
- Modify: `boot/task.c` — add `MenuBarTask`
- Modify: `boot/main.c` — register `MenuBarTask` at priority 0

Menu bar: full-width 20px tall black strip at y=0. Shows "SaphireOS" label left-aligned. No click actions in this task.

- [ ] **Step 1: Add `DrawMenuBar()` to `boot/graphics_elements.c`**

Add at end of file before closing:

```c
void DrawMenuBar() {
    VBEInfoBlock* VBE = (VBEInfoBlock*) VBEInfoAddress;
    // Black bar full width, 20px tall
    DrawRect(0, 0, VBE->x_resolution, 20, 0, 0, 0);
    // "SaphireOS" label — white text (16,32,16 = max brightness)
    char label[] = "SaphireOS";
    DrawString(getArialCharacter, font_arial_width, font_arial_height,
               label, 8, 3, 16, 32, 16);
}
```

- [ ] **Step 2: Declare in `boot/graphics.h`**

Add after existing declarations:

```c
void DrawMenuBar();
```

- [ ] **Step 3: Add `MenuBarTask` to `boot/task.c`**

Add after `ClearScreenTask`:

```c
int MenuBarTask(int taskId) {
    DrawMenuBar();
    return 0;
}
```

- [ ] **Step 4: Register in `boot/main.c` `start()`**

Add after `ClearScreenTask` registration, before `TaskbarTask`:

```c
tasks[TasksLength].priority = 0;
tasks[TasksLength].function = &MenuBarTask;
TasksLength++;
```

- [ ] **Step 5: Build and test**

```bash
make && make run
```

Expected: black 20px bar at top of screen with "SaphireOS" text in mint/white. Rest of OS unchanged.

- [ ] **Step 6: Commit**

```bash
git add boot/graphics_elements.c boot/graphics.h boot/task.c boot/main.c
git commit -m "feat: add macOS-style system menu bar at top of screen"
```

---

### Task 2: Dock (bottom strip with app launcher icons)

**Files:**
- Modify: `boot/graphics_elements.c` — add `DrawDock()`
- Modify: `boot/graphics.h` — declare `DrawDock()`
- Modify: `boot/task.c` — add `DockTask`, remove old `TaskbarTask` shell/ball buttons (or keep as dock items)
- Modify: `boot/main.c` — register `DockTask` at priority 0; remove `TaskbarTask` registration

Dock: 48px tall strip at bottom. Semi-dark background. Centered row of icon buttons (40×40 each, 4px gap). Icons are colored rectangles with labels below (no image assets — bare metal).

- [ ] **Step 1: Add `DrawDock()` to `boot/graphics_elements.c`**

```c
// DrawDock: bottom dock strip. icon_count = number of icons.
// icons[] = flat array: [label_ptr, r, g, b] repeated per icon (4 ints per icon).
// Returns index of clicked icon (0-based), or -1 if none.
// taskId: scope mouse interaction.
int DrawDock(int* icons_r, int* icons_g, int* icons_b,
             char** labels, int icon_count, int taskId) {
    VBEInfoBlock* VBE = (VBEInfoBlock*) VBEInfoAddress;
    int screen_w = VBE->x_resolution;
    int screen_h = VBE->y_resolution;
    int dock_h = 48;
    int icon_size = 36;
    int gap = 8;
    int dock_y = screen_h - dock_h;

    // Dock background: dark translucent strip
    DrawRect(0, dock_y, screen_w, dock_h, 2, 4, 2);

    // Center icons
    int total_w = icon_count * (icon_size + gap) - gap;
    int start_x = (screen_w - total_w) / 2;

    for (int i = 0; i < icon_count; i++) {
        int ix = start_x + i * (icon_size + gap);
        int iy = dock_y + (dock_h - icon_size) / 2;

        // Icon button
        if (DrawButton(ix, iy, icon_size, icon_size,
                       icons_r[i], icons_g[i], icons_b[i],
                       labels[i], 16, 32, 16, taskId) == TRUE) {
            return i;
        }
    }
    return -1;
}
```

- [ ] **Step 2: Declare in `boot/graphics.h`**

```c
int DrawDock(int* icons_r, int* icons_g, int* icons_b,
             char** labels, int icon_count, int taskId);
```

- [ ] **Step 3: Add `DockTask` to `boot/task.c`**

DockTask uses `tasks[taskId].ca1` (100-byte char buffer) as scratch and iparams for launch counter. Define icon colors and labels as static locals (avoids heap):

```c
int DockTask(int taskId) {
    // 3 dock icons: Shell (blue), Ball (green), Term (purple)
    int rs[3] = {0,  0, 8};
    int gs[3] = {10, 20, 0};
    int bs[3] = {16, 0, 16};
    char l0[] = "Sh";
    char l1[] = "Bl";
    char l2[] = "Tm";
    char* labels[3] = {l0, l1, l2};

    int clicked = DrawDock(rs, gs, bs, labels, 3, taskId);

    int i = iparams[taskId * task_params_length + 0]; // launch offset

    if (clicked == 0) { // Shell
        tasks[TasksLength].priority = 1;
        tasks[TasksLength].taskId = TasksLength;
        tasks[TasksLength].function = &ShellTask;
        iparams[TasksLength * task_params_length + 0] = 60 + i * 20;
        iparams[TasksLength * task_params_length + 1] = 60 + i * 20;
        iparams[TasksLength * task_params_length + 2] = 280;
        iparams[TasksLength * task_params_length + 3] = 240;
        iparams[TasksLength * task_params_length + 4] = 0;
        iparams[TasksLength * task_params_length + 5] = 0;
        iparams[TasksLength * task_params_length + 6] = 0;
        TasksLength++;
        iparams[taskId * task_params_length + 0]++;
    }
    if (clicked == 1) { // Ball
        tasks[TasksLength].priority = 1;
        tasks[TasksLength].taskId = TasksLength;
        tasks[TasksLength].function = &BallTask;
        iparams[TasksLength * task_params_length + 0] = 60 + i * 20;
        iparams[TasksLength * task_params_length + 1] = 60 + i * 20;
        iparams[TasksLength * task_params_length + 2] = 280;
        iparams[TasksLength * task_params_length + 3] = 240;
        iparams[TasksLength * task_params_length + 5] = 20;
        iparams[TasksLength * task_params_length + 6] = 30;
        iparams[TasksLength * task_params_length + 7] = 5;
        iparams[TasksLength * task_params_length + 8] = 5;
        TasksLength++;
        iparams[taskId * task_params_length + 0]++;
    }
    return 0;
}
```

- [ ] **Step 4: Update `boot/main.c` — replace TaskbarTask with DockTask**

Remove:
```c
tasks[TasksLength].priority = 0;
tasks[TasksLength].function = &TaskbarTask;
tasks[TasksLength].taskId = TasksLength;
iparams[TasksLength * task_params_length + 0] = 0;
iparams[TasksLength * task_params_length + 1] = 0;
iparams[TasksLength * task_params_length + 2] = VBE->x_resolution;
iparams[TasksLength * task_params_length + 3] = 40;
iparams[TasksLength * task_params_length + 4] = 1;
TasksLength++;
```

Add:
```c
tasks[TasksLength].priority = 0;
tasks[TasksLength].function = &DockTask;
tasks[TasksLength].taskId = TasksLength;
iparams[TasksLength * task_params_length + 0] = 0; // launch counter
TasksLength++;
```

- [ ] **Step 5: Build and test**

```bash
make && make run
```

Expected: dark strip at bottom with 3 colored icon buttons ("Sh", "Bl", "Tm"). Click Sh → Shell window appears. Click Bl → Ball window appears. Menu bar still visible at top.

- [ ] **Step 6: Commit**

```bash
git add boot/graphics_elements.c boot/graphics.h boot/task.c boot/main.c
git commit -m "feat: add macOS-style dock at bottom with app launcher icons"
```

---

### Task 3: Traffic-light window buttons (macOS-style)

**Files:**
- Modify: `boot/graphics_elements.c` — add `DrawWindowMac()` replacing close-right pattern with red/yellow/green circles at top-left
- Modify: `boot/graphics.h` — declare `DrawWindowMac()`
- Modify: `boot/task.c` — update `ShellTask` and `BallTask` to call `DrawWindowMac` instead of `DrawWindow`

macOS windows: title bar 20px at top, 3 circles at x+8/x+24/x+40, y+10, radius 6. Red=close, Yellow=minimize (no-op for now), Green=maximize (no-op). Returns 1 on close click (same as `DrawWindow`).

- [ ] **Step 1: Add `DrawWindowMac()` to `boot/graphics_elements.c`**

```c
// DrawWindowMac: macOS-style window with traffic-light buttons top-left.
// Returns 1 if close (red) clicked, 0 otherwise.
// x,y,w,h: position/size (x and y are pointers — draggable).
// mouse_held: drag state (pass iparams slot).
int DrawWindowMac(int* x, int* y, int* w, int* h,
                  int r, int g, int b,
                  int* mouse_held, int taskId) {
    // Title bar
    DrawRect(*x, *y, *w, 20, 3, 6, 3);
    // Window body
    DrawRect(*x, *y + 20, *w, *h - 20, r, g, b);

    // Drag: click on title bar (excluding first 60px for buttons)
    if (*mouse_held == TRUE ||
        (left_clicked == TRUE &&
         mx > *x + 60 && mx < *x + *w &&
         my > *y && my < *y + 20 &&
         mouse_possessed_task_id == taskId)) {
        *mouse_held = TRUE;
        *x += mouse_dx;
        *y += mouse_dy;
    }
    if (left_clicked == FALSE) *mouse_held = FALSE;

    // Red close button (x+8, y+10)
    if (DrawCircleButton(*x + 8, *y + 10, 6, 16, 0, 0, taskId) == TRUE)
        return 1;

    // Yellow minimize (x+24, y+10) — no-op
    DrawCircleButton(*x + 24, *y + 10, 6, 16, 20, 0, taskId);

    // Green maximize (x+40, y+10) — no-op
    DrawCircleButton(*x + 40, *y + 10, 6, 0, 20, 0, taskId);

    return 0;
}
```

Note: `mouse_dx` and `mouse_dy` must be exposed — they are the per-frame mouse delta. Read `boot/input.c` to confirm global names. If only `mx`/`my` exist (no delta), compute delta: store previous `mx`/`my` in iparams slots 8–9, compute `dx = mx - prev_mx`.

- [ ] **Step 2: Verify mouse delta globals in `boot/input.c`**

```bash
grep -n "mouse_dx\|mouse_dy\|prev_mx\|prev_my\|dx\|dy" boot/input.c
```

If `mouse_dx`/`mouse_dy` don't exist: open `boot/input.c`, find where `mx`/`my` are updated in `HandleISR12`. Add delta tracking there:

```c
// In HandleISR12, after updating mx and my:
mouse_dx = (signed char)byte2;
mouse_dy = -(signed char)byte3;
```

Declare in `boot/graphics.h`:
```c
extern int mouse_dx;
extern int mouse_dy;
```

- [ ] **Step 3: Declare `DrawWindowMac()` in `boot/graphics.h`**

```c
int DrawWindowMac(int* x, int* y, int* w, int* h,
                  int r, int g, int b,
                  int* mouse_held, int taskId);
```

- [ ] **Step 4: Update `ShellTask` to use `DrawWindowMac`**

In `boot/task.c`, `ShellTask`, change:
```c
int closeClicked = DrawWindow(
```
to:
```c
int closeClicked = DrawWindowMac(
```

Same arguments — signature matches.

- [ ] **Step 5: Update `BallTask` to use `DrawWindowMac`**

Same change in `BallTask`:
```c
int closeClicked = DrawWindowMac(
```

- [ ] **Step 6: Build and test**

```bash
make && make run
```

Open Shell and Ball windows from dock. Expected: 3 colored circles top-left (red/yellow/green). Red circle closes window. Dragging by title bar (after buttons) moves window. Yellow/green circles are visible but do nothing.

- [ ] **Step 7: Commit**

```bash
git add boot/graphics_elements.c boot/graphics.h boot/task.c boot/input.c
git commit -m "feat: macOS traffic-light window buttons (red/yellow/green top-left)"
```

---

### Task 4: Text input widget

**Files:**
- Modify: `boot/graphics_elements.c` — add `DrawTextInput()`
- Modify: `boot/graphics.h` — declare `DrawTextInput()`

Text input: bordered rectangle. Shows cursor `|` at end when focused. Reads `Scancode` global when focused. Updates caller-provided `char*` buffer.

iparams constraint: caller must pass buffer pointer via `tasks[taskId].ca1` (100-byte field) — no heap.

- [ ] **Step 1: Add `DrawTextInput()` to `boot/graphics_elements.c`**

```c
// DrawTextInput: text input box.
// buf: char buffer to write into (max buflen-1 chars).
// buflen: buffer capacity.
// focused: 1 if this input has keyboard focus.
// Returns 1 if user pressed Enter, 0 otherwise.
int DrawTextInput(int x, int y, int w, int h,
                  char* buf, int buflen, int focused, int taskId) {
    // Background
    int br = focused ? 2 : 1;
    int bg = focused ? 4 : 2;
    int bb = focused ? 2 : 1;
    DrawRect(x, y, w, h, br, bg, bb);
    // Border (1px inset — draw rect outline manually)
    DrawRect(x, y, w, 1, 0, 10, 0);       // top
    DrawRect(x, y + h - 1, w, 1, 0, 10, 0); // bottom
    DrawRect(x, y, 1, h, 0, 10, 0);       // left
    DrawRect(x + w - 1, y, 1, h, 0, 10, 0); // right

    // Draw text content
    DrawString(getArialCharacter, font_arial_width, font_arial_height,
               buf, x + 3, y + (h - font_arial_height) / 2, 16, 32, 16);

    // Cursor blink: always show when focused (no timer — bare metal)
    if (focused) {
        int len = 0;
        while (buf[len]) len++;
        int cx = x + 3 + len * font_arial_width;
        DrawRect(cx, y + 3, 1, h - 6, 16, 32, 16);
    }

    // Handle keyboard input when focused
    if (focused) {
        char c = ProcessScancode(Scancode);
        if (backspace_pressed == TRUE) {
            int len = 0;
            while (buf[len]) len++;
            if (len > 0) { buf[len - 1] = '\0'; }
            backspace_pressed = FALSE;
            Scancode = -1;
        } else if (c == '\n' || c == '\r') {
            Scancode = -1;
            return 1; // Enter pressed
        } else if (c != '\0') {
            int len = 0;
            while (buf[len]) len++;
            if (len < buflen - 1) {
                buf[len] = c;
                buf[len + 1] = '\0';
            }
            Scancode = -1;
        }
    }

    return 0;
}
```

- [ ] **Step 2: Declare in `boot/graphics.h`**

```c
int DrawTextInput(int x, int y, int w, int h,
                  char* buf, int buflen, int focused, int taskId);
```

- [ ] **Step 3: Add text input to `ShellTask` for demo**

In `boot/task.c`, `ShellTask`, after buttons, add:

```c
char* inputBuf = tasks[taskId].ca1;
int entered = DrawTextInput(
    x + 10, y + 60, width - 20, 20,
    inputBuf, 100, 1, taskId);
// Display submitted text below input
DrawString(getArialCharacter, font_arial_width, font_arial_height,
           inputBuf, x + 10, y + 90, 16, 32, 16);
```

Note: `ca1` is 100 bytes. `buflen=100` matches. Remove old `HandleKeyboardTask` keyboard echo if it conflicts.

- [ ] **Step 4: Build and test**

```bash
make && make run
```

Open Shell window. Expected: text input box visible in window body. Type keyboard chars → appear in input. Backspace works. Text renders live inside box.

- [ ] **Step 5: Commit**

```bash
git add boot/graphics_elements.c boot/graphics.h boot/task.c
git commit -m "feat: add DrawTextInput widget for in-window keyboard text entry"
```

---

### Task 5: Checkbox widget

**Files:**
- Modify: `boot/graphics_elements.c` — add `DrawCheckbox()`
- Modify: `boot/graphics.h` — declare `DrawCheckbox()`

Checkbox: 14×14 bordered box, filled with checkmark `X` pattern when checked. Click toggles `*checked`. Returns 1 on click.

- [ ] **Step 1: Add `DrawCheckbox()` to `boot/graphics_elements.c`**

```c
// DrawCheckbox: toggleable checkbox with label.
// checked: pointer to int (0=unchecked, 1=checked).
// Returns 1 if state toggled this frame, 0 otherwise.
int DrawCheckbox(int x, int y, int* checked, char* label, int taskId) {
    int size = 14;
    // Box background
    DrawRect(x, y, size, size, 1, 2, 1);
    // Border
    DrawRect(x, y, size, 1, 0, 16, 0);
    DrawRect(x, y + size - 1, size, 1, 0, 16, 0);
    DrawRect(x, y, 1, size, 0, 16, 0);
    DrawRect(x + size - 1, y, 1, size, 0, 16, 0);

    // Checkmark: two diagonal lines forming X
    if (*checked) {
        for (int i = 2; i < size - 2; i++) {
            Draw(x + i, y + i, 0, 32, 0);
            Draw(x + (size - 1 - i), y + i, 0, 32, 0);
        }
    }

    // Label to the right
    DrawString(getArialCharacter, font_arial_width, font_arial_height,
               label, x + size + 4, y, 16, 32, 16);

    // Click detection
    if (mouse_possessed_task_id == taskId &&
        left_clicked == TRUE &&
        mx >= x && mx <= x + size &&
        my >= y && my <= y + size) {
        *checked = !(*checked);
        left_clicked = FALSE;
        return 1;
    }
    return 0;
}
```

- [ ] **Step 2: Declare in `boot/graphics.h`**

```c
int DrawCheckbox(int x, int y, int* checked, char* label, int taskId);
```

- [ ] **Step 3: Add checkbox demo to `ShellTask`**

In `boot/task.c`, `ShellTask`:

Add iparams slot usage comment at top of ShellTask:
```
// iparams slots: 0=x, 1=y, 2=w, 3=h, 4=r, 5=g, 6=b, 7=checkbox_state, 8=mouse_prev_x, 9=mouse_held
```

Add after buttons:
```c
int* darkMode = &iparams[taskId * task_params_length + 7];
char cbLabel[] = "Dark mode";
DrawCheckbox(x + 20, y + 50, darkMode, cbLabel, taskId);
if (*darkMode) {
    *r = 0; *g = 0; *b = 0;
} else {
    *r = 16; *g = 31; *b = 16;
}
```

Note: this replaces the Dark/Light buttons. Remove `DrawButton` calls for "Dark" and "Light" in ShellTask after adding checkbox, to free iparams conflict on slot 4–6.

- [ ] **Step 4: Build and test**

```bash
make && make run
```

Open Shell window. Expected: checkbox with label "Dark mode". Click → toggles check state and changes window background color.

- [ ] **Step 5: Commit**

```bash
git add boot/graphics_elements.c boot/graphics.h boot/task.c
git commit -m "feat: add DrawCheckbox widget with toggle state and label"
```

---

### Task 6: Window z-order (bring-to-front on click)

**Files:**
- Modify: `boot/task.c` — `ProcessTasks()` already raises clicked task to priority 2. Enforce: menu bar + dock always priority 0; mouse always priority 5; windows use priority 1 (background) or 2 (focused).

Current behavior: clicking a window raises its priority to 2. Unfocused windows reset to 0 (same as menu bar, drawn over by menu bar). Fix: unfocused windows should be priority 1, not 0.

- [ ] **Step 1: Read `ProcessTasks()` focus logic**

Re-read lines 21–61 of `boot/task.c`. Confirm: `tasks[mouse_possessed_task_id].priority = 0` resets to 0.

- [ ] **Step 2: Change reset priority from 0 to 1**

In `boot/task.c` `ProcessTasks()`, line:
```c
tasks[mouse_possessed_task_id].priority = 0;
```
Change to:
```c
tasks[mouse_possessed_task_id].priority = 1;
```

- [ ] **Step 3: Update `DockTask` and `MenuBarTask` registration to ensure priority 0**

In `boot/main.c`, verify `MenuBarTask` and `DockTask` are registered with `priority = 0`. They should never be reassigned priority by `ProcessTasks` because dock/menu bar iparams cover the full screen edge — protect by checking `taskId` in click focus logic.

Add guard in `ProcessTasks()` before `tasks[i].priority = 2`:
```c
if (tasks[i].function == &MenuBarTask ||
    tasks[i].function == &DockTask ||
    tasks[i].function == &DrawMouseTask) continue;
```

- [ ] **Step 4: Update initial window launch priority**

In `DockTask`, change window launch priority from 1 to 1 (already correct — confirm in Task 2 code above).

- [ ] **Step 5: Build and test**

```bash
make && make run
```

Open two windows. Click between them. Expected: clicked window draws on top (priority 2), other window draws behind (priority 1). Menu bar always visible at top. Dock always visible at bottom.

- [ ] **Step 6: Commit**

```bash
git add boot/task.c
git commit -m "fix: window z-order — unfocused windows priority 1, protect system UI from focus steal"
```
