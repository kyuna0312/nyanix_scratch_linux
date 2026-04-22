# ARM64 Nyanix Makefile

CC = aarch64-linux-gnu-gcc
OBJCOPY = aarch64-linux-gnu-objcopy

CFLAGS = -Os -Wall -nostdinc -ffreestanding -fno-pie -mgeneral-regs-only -static
LDFLAGS = -nostdlib -static -T kernel.ld

SRC = main.c boot.S

all: kernel.elf kernel.bin kernel8.img

kernel.elf: $(SRC) kernel.ld
	$(CC) $(CFLAGS) $(LDFLAGS) -o kernel.elf $(SRC)

kernel.bin: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel.bin

kernel8.img: kernel.bin
	cp kernel.bin kernel8.img

run: kernel.elf
	qemu-system-aarch64 -M virt -m 256 -cpu cortex-a57 \
	  -kernel kernel.elf \
	  -serial mon:stdio \
	  -nographic

clean:
	rm -f kernel.elf kernel.bin kernel8.img