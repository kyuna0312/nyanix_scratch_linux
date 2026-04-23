# x86_64 Nyanix Makefile

CC = x86_64-linux-gnu-gcc
AS = nasm
OBJCOPY = objcopy

CFLAGS = -Os -Wall -nostdinc -ffreestanding -fno-pie -static -m64
LDFLAGS = -nostdlib -T kernel/kernel.ld

all: x64.img boot-arm64.img

boot-x64.elf: kernel/main.c kernel/kernel.ld | bin
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ kernel/main.c

x64.img: boot.S boot-x64.elf
	$(AS) boot.S -f bin -o boot.bin
	cat boot.bin boot-x64.elf > x64.img

boot-arm64.elf: 
	$(MAKE) -C ../boot-arm64

boot-arm64.img: boot-arm64.elf
	cp ../boot-arm64/bin/kernel8.img $@

run: x64.img
	qemu-system-x86_64 -m 256 -drive format=raw,file=x64.img -nographic

run-x64: x64.img
	qemu-system-x86_64 -m 256 -drive format=raw,file=x64.img -nographic

run-arm64: boot-arm64.img
	qemu-system-aarch64 -M virt -m 256 -cpu cortex-a57 -kernel boot-arm64.img -nographic

clean:
	rm -rf bin/*.elf x64.img boot-arm64.img boot.bin

.PHONY: all run run-x64 run-arm64 clean