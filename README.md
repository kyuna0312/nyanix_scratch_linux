# Nyanix — SaphireOS

Bare-metal x86 OS built from scratch. Boots from a custom MBR, runs in VBE 640×480 16bpp (RGB565), and drives a cooperative task scheduler with PS/2 mouse + keyboard input. No libc, no OS abstractions.

Built for learning low-level systems: bootloaders, VBE graphics, IDT/ISR wiring, and cooperative multitasking.

---

## Quick Start

```bash
# 1. Install dependencies (Arch/Manjaro)
./scripts/setup.sh

# 2. Build
./scripts/build.sh

# 3. Run in QEMU
./scripts/run.sh

# Or with make
make
make run
```

---

## What It Does Right Now

| Feature | Status |
|---|---|
| MBR bootloader | done |
| VBE 640×480 16bpp framebuffer | done |
| Off-screen buffer + `Flush()` | done |
| Custom Arial bitmap font renderer | done |
| PS/2 keyboard (IDT IRQ1) | done |
| PS/2 mouse (IDT IRQ12) | done |
| Cooperative task scheduler (priority 0–5) | done |
| PIT at ~100 Hz for frame pacing | done |
| Draggable windows with title bar + close button | done |
| Hover/click buttons scoped to focused task | done |
| Taskbar | done |

---

## Project Structure

```
nyanix_scratch_linux/
├── boot/
│   ├── boot.asm             # MBR bootloader — sets up VBE, loads kernel
│   ├── kernel_entry.asm     # 32-bit entry point, calls start()
│   ├── final.c              # Root compilation unit — #includes all .c in order
│   ├── graphics.c / .h      # Draw(), DrawRect(), DrawString(), Flush()
│   ├── font.c               # Bitmap Arial font data + getArialCharacter()
│   ├── input.c              # IDT setup, keyboard/mouse ISRs, PIT init
│   ├── graphics_elements.c  # DrawWindow(), DrawButton(), DrawTaskbar()
│   ├── task.c               # Task struct, ProcessTasks() scheduler
│   ├── main.c               # start() — task registration + main loop
│   ├── bin/                 # Build artifacts (gitignored)
│   └── utilities/           # Font generation tools
├── scripts/
│   ├── setup.sh             # Install deps via pacman
│   ├── build.sh             # Full build pipeline
│   └── run.sh               # Stale-check build + launch QEMU
├── makefile
└── os.img                   # Final bootable image (gitignored)
```

---

## Architecture Notes

**Compilation model** — single translation unit. `final.c` `#include`s every `.c` file in a fixed order. Globals defined in earlier files are visible to later ones (e.g. `input.c` globals used in `task.c` and `main.c`).

**Memory map (fixed)**
| Address | Purpose |
|---|---|
| `0x7c00` | MBR |
| `0x1000` | Kernel loads here |
| `0x8000` | VBEInfoBlock |
| `0x90000` | Stack top |
| `0xffff0` | Off-screen framebuffer (RGB565) |

**Color** — RGB565: `r<<11 | g<<5 | b`. Ranges: R 0–16, G 0–32, B 0–16.

**Task system** — `tasks[256]`, priority 0 (highest) to 5. Params via `iparams[taskId * 10 + N]`. Max ~10 tasks (100 int param slots total). No dynamic allocation.

---

## Known Constraints

- String literals ≤ 61 chars (stack-allocated char arrays, no heap)
- ~10 task limit from `iparams[100]`
- No dynamic memory allocation
- Single CPU, cooperative (no preemption)
- VBE fixed at mode `0x111` (640×480 16bpp)

---

## Future Ideas

### Near-term
- [ ] `iparams` bump to 256 slots — lift the ~10 task limit
- [ ] Double-buffering: only `Flush()` dirty regions instead of full screen
- [ ] Resizable windows — pass width/height as iparams, handle drag corners
- [ ] Basic text input widget — character buffer task with cursor blink
- [ ] Mouse cursor: custom sprite instead of filled rect

### Mid-term
- [ ] Simple filesystem — FAT12 on the disk image to load programs
- [ ] Flat memory allocator — fixed-size block pool (no `malloc`, but less wasteful)
- [ ] More fonts — monospace for terminal-style apps, variable-width support
- [ ] Window z-ordering — click to bring forward, track `mouse_possessed_task_id` properly
- [ ] Color themes — palette abstraction over RGB565 constants

### Long-term / exploratory
- [ ] Protected mode segments for basic task isolation
- [ ] Preemptive scheduler with PIT-driven context switch
- [ ] ATA PIO disk driver — read sectors directly from IDE
- [ ] VBE mode switch UI — let the user pick resolution at boot
- [ ] Serial port output — debug logging via COM1 to QEMU stdio
- [ ] Soundblaster 16 / PC speaker beeps for feedback

---

## Dependencies

| Tool | Purpose |
|---|---|
| `nasm` | Assemble MBR + kernel entry |
| `gcc` (i686 / multilib) | Compile 32-bit freestanding C |
| `binutils` (`ld`, `objcopy`) | Link + strip to raw binary |
| `qemu-full` | Run the image |
