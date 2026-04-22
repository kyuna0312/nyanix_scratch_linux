#!/bin/bash
# Build Nyanix kernel
set -e

mkdir -p boot/bin

echo "Assembling bootloader..."
nasm boot/boot.asm -f bin -o boot/bin/boot.bin

echo "Assembling kernel entry..."
nasm boot/kernel_entry.asm -f elf -o boot/bin/kernel_entry.o

echo "Compiling kernel..."
gcc -m32 -ffreestanding -fno-stack-protector -nostdlib \
    -fno-pic -fno-pie -fno-asynchronous-unwind-tables \
    -Wno-write-strings \
    -Iboot \
    -c boot/final.c -o boot/bin/final.o

echo "Linking kernel..."
ld -m elf_i386 -T boot/kernel.ld \
    -o boot/bin/kernel.elf \
    boot/bin/kernel_entry.o boot/bin/final.o

echo "Creating image..."
objcopy -O binary boot/bin/kernel.elf boot/bin/kernel.bin
cat boot/bin/boot.bin boot/bin/kernel.bin > os.img

echo "Done: os.img ($(ls -lh os.img | awk '{print $5}'))"
