# ARM64 Nyanix Makefile

CC = aarch64-linux-gnu-gcc
OBJCOPY = aarch64-linux-gnu-objcopy

CFLAGS = -Os -Wall -nostdinc -ffreestanding -fno-pie -mgeneral-regs-only -static

KERNEL_SRC = kernel/main.c kernel/boot.S
KERNEL_BIN = bin/kernel.elf bin/kernel.bin bin/kernel8.img

all: $(KERNEL_BIN)

bin/kernel.elf: $(KERNEL_SRC) kernel/kernel.ld
	$(CC) $(CFLAGS) -nostdlib -T kernel/kernel.ld -o $@ $(KERNEL_SRC)

bin/kernel.bin: bin/kernel.elf
	$(OBJCOPY) -O binary $< $@

bin/kernel8.img: bin/kernel.bin
	cp $< $@

run: bin/kernel.elf
	qemu-system-aarch64 -M virt -m 256 -cpu cortex-a57 -kernel $< -nographic

clean:
	rm -f bin/*.elf bin/*.bin bin/*.img