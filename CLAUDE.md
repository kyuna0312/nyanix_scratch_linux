# CLAUDE.md — Nyanix (SaphireOS)

Bare-metal x86 OS. VBE mode 0x111 (640x480 16bpp). Cooperative task loop. No libc.

## Commands
```bash
make        # build os.img
make run    # build + QEMU
make clear  # clean
```

## Build
`boot/boot.asm` → MBR → `boot/kernel_entry.asm` → calls `start()` → `boot/final.c` (32-bit freestanding) → `os.img`  
Kernel loads at `0x1000`. Stack at `0x90000`.

## Compilation Model
`boot/final.c` `#include`s all `.c` in order: `graphics.c → font.c → input.c → graphics_elements.c → task.c → main.c`. No separate compilation. Globals from `input.c` shared by inclusion order.

## Memory (fixed)
- `0x7c00` MBR, `0x8000` VBEInfoBlock, `0x1000` kernel entry
- `0xffff0` off-screen buffer (RGB565); `Flush()` copies → `VBE->screen_ptr`

## Color
RGB565: `r<<11 | g<<5 | b`. Ranges: R 0–16, G 0–32, B 0–16. Not 0–255.

## Tasks (`task.c`)
`struct Task tasks[256]`, priority 0–5 (0=highest), `int (*function)(int taskId)`.  
Params: `iparams[taskId * task_params_length + N]` (10 slots/task).  
`mouse_possessed_task_id` — tasks must check before consuming clicks.

## Input (`input.c`)
IDT 1=keyboard, 12=PS/2 mouse. `Scancode` global set by `HandleISR1`. Mouse: 3-byte packets → `mx`/`my`/`left_clicked`/`right_clicked`.

## UI (`graphics_elements.c`)
- `DrawWindow(taskId)` — draggable, title bar 20px, close circle. Returns 1 on close.
- `DrawButton(taskId)` — hover+click, scoped to `mouse_possessed_task_id`.

## Font (`font.c`)
`getArialCharacter(charIndex, row)` → pass as fn ptr to `DrawString()`/`DrawCharacter()` with `font_arial_width`/`font_arial_height`.

## Constraints
- String literals ≤61 chars (stack-allocated, no libc)
- `iparams` = 100 ints total → max ~10 tasks
- No dynamic allocation
- VBE fixed at `0x111`
