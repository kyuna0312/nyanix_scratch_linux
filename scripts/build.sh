#!/usr/bin/env bash
# build.sh — compile and link Nyanix into os.img
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/boot/bin"

echo "[build] Creating output dir..."
mkdir -p "$BIN"

echo "[build] Assembling MBR bootloader..."
nasm "$ROOT/boot/boot.asm" -f bin -o "$BIN/boot.bin"

echo "[build] Assembling kernel entry..."
nasm "$ROOT/boot/kernel_entry.asm" -f elf -o "$BIN/kernel_entry.o"

echo "[build] Compiling kernel (32-bit freestanding)..."
gcc -m32 -ffreestanding -fno-stack-protector -nostdlib \
    -I"$ROOT/boot" \
    -c "$ROOT/boot/final.c" -o "$BIN/kernel.o"

echo "[build] Linking kernel..."
ld -m elf_i386 -o "$BIN/kernel.elf" \
    -Ttext 0x1000 \
    "$BIN/kernel_entry.o" "$BIN/kernel.o"

echo "[build] Stripping to raw binary..."
objcopy -O binary -j .text "$BIN/kernel.elf" "$BIN/kernel.bin"

echo "[build] Assembling final image..."
cat "$BIN/boot.bin" "$BIN/kernel.bin" > "$ROOT/os.img"

echo "[build] Done → os.img ($(du -h "$ROOT/os.img" | cut -f1))"
