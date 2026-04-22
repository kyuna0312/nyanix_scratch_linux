# Nyanix — SaphireOS

Bare-metal x86 OS built from scratch. Boots from MBR, runs VBE 640×480 16bpp (RGB565), with cooperative multitasking and PS/2 mouse/keyboard input. No libc.

---

## Quick Start

```bash
# Build
./build.sh

# Run in QEMU
./run.sh
```

---

## Features

| Feature | Status |
|---------|---------|
| MBR bootloader | ✅ |
| VBE 640×480 16bpp | ✅ |
| Off-screen buffer + Flush() | ✅ |
| Custom bitmap font renderer | ✅ |
| PS/2 keyboard (IDT IRQ1) | ✅ |
| PS/2 mouse (IDT IRQ12) | ✅ |
| Cooperative task scheduler | ✅ |
| PIT at ~100 Hz | ✅ |
| Draggable windows | ✅ |
| Buttons (hover/click) | ✅ |
| Taskbar with clock | ✅ |
| Desktop background | ✅ |

---

## Project Structure

```
nyanix_scratch_linux/
├── boot/
│   ├── boot.asm           # MBR bootloader
│   ├── kernel_entry.asm  # 32-bit entry point
│   ├── kernel.ld         # Linker script
│   ├── main.c           # start() + loop
│   ├── task.c            # Tasks (Shell, Ball, Desktop, Clock)
│   ├── graphics.c/h     # Draw(), DrawRect(), Flush()
│   ├── input.c          # Keyboard/mouse ISRs
│   ├── graphics_elements.c # UI elements
│   ├── font.c           # Bitmap font
│   ├── final.c          # Combined entry
│   └── bin/             # Build output
├── build.sh             # Build script
├── run.sh              # Run script
├── makefile            # Make build
└── os.img             # Built image
```

---

## Architecture

**Memory Map**
| Address | Purpose |
|---------|---------|
| 0x7c00 | MBR |
| 0x1000 | Kernel |
| 0x8000 | VBEInfoBlock |
| 0x90000 | Stack |
| 0xffff0 | Framebuffer |

**Color** — RGB565: `r<<11 | g<<5 | b`

**Task System** — `tasks[256]`, priority 0-5. Params via `iparams[taskId * 10 + N]`.

---

## Constraints

- String literals ≤ 61 chars
- Max ~10 tasks (100 param slots)
- No dynamic allocation
- Single CPU, cooperative multitasking
- VBE fixed at 0x111 (640×480 16bpp)

---

## Build Tools

| Tool | Purpose |
|------|---------|
| nasm | Assemble bootloader |
| gcc -m32 | Compile 32-bit freestanding |
| ld, objcopy | Link + binary |
| qemu-system-x86_64 | Run |

---

## Building

```bash
# Option 1: build.sh
./build.sh

# Option 2: Make
make
make run

# Clean
make clean
```

---

## Future Ideas

- FAT12 filesystem
- Text input widget
- Window resizing
- Z-ordering
- Sound (PC speaker)
- ATA disk driver