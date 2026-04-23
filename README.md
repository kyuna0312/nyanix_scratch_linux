# Nyanix — Bare-Metal OS

Multi-architecture bare-metal OS: x86, x86_64, ARM64.

## Build & Run

```bash
make      # Build all
make run       # Run x86_64
make run-arm64 # Run ARM64
```

## Variants

| Arch | Path | Display |
|------|------|---------|
| x86 (32-bit) | `boot/` | VBE 640×480 16bpp |
| x86_64 | `boot-x64/` | VGA text + UART |
| ARM64 | `boot-arm64/` | UART |

## Quick Start

```bash
# x86_64
make run

# ARM64 (QEMU virt)
make run-arm64
```

## Project Structure

```
nyanix/
├── boot/          # 32-bit x86 (VBE graphics)
├── boot-x64/      # x86_64 bare-metal
├── boot-arm64/     # ARM64 bare-metal
├── Makefile       # Unified build
└── os.img       # 32-bit disk image
```

## Features (x86 32-bit)

- MBR bootloader + VBE graphics
- Cooperative multitasking (256 tasks, priority 0-5)
- PS/2 keyboard/mouse with IDT
- Draggable windows, buttons, taskbar
- Custom bitmap font renderer

## Memory Map (32-bit)

| Address | Purpose |
|---------|---------|
| 0x7c00 | MBR |
| 0x1000 | Kernel |
| 0x8000 | VBEInfoBlock |
| 0xffff0 | Framebuffer |

## Requirements

- nasm, gcc (32-bit + 64-bit cross-compilers)
- qemu-system-x86_64, qemu-system-aarch64