BIN = boot/bin

.PHONY: all run clean

all: $(BIN)/boot.bin $(BIN)/kernel.bin
	cat $(BIN)/boot.bin $(BIN)/kernel.bin > os.img

$(BIN):
	mkdir -p $(BIN)

$(BIN)/boot.bin: boot/boot.asm | $(BIN)
	nasm boot/boot.asm -f bin -o $(BIN)/boot.bin

$(BIN)/kernel_entry.o: boot/kernel_entry.asm | $(BIN)
	nasm boot/kernel_entry.asm -f elf -o $(BIN)/kernel_entry.o

$(BIN)/kernel.o: boot/final.c boot/*.c boot/*.h | $(BIN)
	gcc -m32 -ffreestanding -fno-stack-protector -nostdlib \
	    -I boot -c boot/final.c -o $(BIN)/kernel.o

$(BIN)/kernel.elf: $(BIN)/kernel_entry.o $(BIN)/kernel.o
	ld -m elf_i386 -o $(BIN)/kernel.elf -Ttext 0x1000 \
	    $(BIN)/kernel_entry.o $(BIN)/kernel.o

$(BIN)/kernel.bin: $(BIN)/kernel.elf
	objcopy -O binary -j .text $(BIN)/kernel.elf $(BIN)/kernel.bin

run: all
	qemu-system-x86_64 -drive format=raw,file=os.img -m 32M -vga std -no-reboot

clean:
	rm -rf $(BIN) os.img os.img.lock
