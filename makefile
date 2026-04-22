BIN = boot/bin
LOADER_SRC = boot
KERNEL_SRC = boot

.PHONY: all run clean

all: $(BIN)/boot.bin $(BIN)/kernel.bin
	cat $(BIN)/boot.bin $(BIN)/kernel.bin > os.img

$(BIN):
	mkdir -p $(BIN)

# Bootloader build (remains assembly)
$(BIN)/boot.bin: $(LOADER_SRC)/boot.asm | $(BIN)
	nasm $(LOADER_SRC)/boot.asm -f bin -o $(BIN)/boot.bin

$(BIN)/kernel_entry.o: $(LOADER_SRC)/kernel_entry.asm | $(BIN)
	nasm $(LOADER_SRC)/kernel_entry.asm -f elf -o $(BIN)/kernel_entry.o

# Kernel build (separate compilation of each module)
$(BIN)/final.o: $(KERNEL_SRC)/final.c | $(BIN)
	gcc -m32 -ffreestanding -fno-stack-protector -nostdlib \
	    -fno-pic -fno-pie -fno-asynchronous-unwind-tables \
	    -Wno-write-strings \
	    -I$(KERNEL_SRC) \
	    -c $(KERNEL_SRC)/final.c -o $(BIN)/final.o

$(BIN)/graphics.o: $(KERNEL_SRC)/graphics.c $(KERNEL_SRC)/graphics.h | $(BIN)
	gcc -m32 -ffreestanding -fno-stack-protector -nostdlib \
	    -fno-pic -fno-pie -fno-asynchronous-unwind-tables \
	    -Wno-write-strings \
	    -I$(KERNEL_SRC) \
	    -c $(KERNEL_SRC)/graphics.c -o $(BIN)/graphics.o

$(BIN)/font.o: $(KERNEL_SRC)/font.c $(KERNEL_SRC)/graphics.h | $(BIN)
	gcc -m32 -ffreestanding -fno-stack-protector -nostdlib \
	    -fno-pic -fno-pie -fno-asynchronous-unwind-tables \
	    -Wno-write-strings \
	    -I$(KERNEL_SRC) \
	    -c $(KERNEL_SRC)/font.c -o $(BIN)/font.o

$(BIN)/input.o: $(KERNEL_SRC)/input.c $(KERNEL_SRC)/graphics.h | $(BIN)
	gcc -m32 -ffreestanding -fno-stack-protector -nostdlib \
	    -fno-pic -fno-pie -fno-asynchronous-unwind-tables \
	    -Wno-write-strings \
	    -I$(KERNEL_SRC) \
	    -c $(KERNEL_SRC)/input.c -o $(BIN)/input.o

$(BIN)/graphics_elements.o: $(KERNEL_SRC)/graphics_elements.c $(KERNEL_SRC)/graphics.h | $(BIN)
	gcc -m32 -ffreestanding -fno-stack-protector -nostdlib \
	    -fno-pic -fno-pie -fno-asynchronous-unwind-tables \
	    -Wno-write-strings \
	    -I$(KERNEL_SRC) \
	    -c $(KERNEL_SRC)/graphics_elements.c -o $(BIN)/graphics_elements.o

$(BIN)/task.o: $(KERNEL_SRC)/task.c $(KERNEL_SRC)/graphics.h | $(BIN)
	gcc -m32 -ffreestanding -fno-stack-protector -nostdlib \
	    -fno-pic -fno-pie -fno-asynchronous-unwind-tables \
	    -Wno-write-strings \
	    -I$(KERNEL_SRC) \
	    -c $(KERNEL_SRC)/task.c -o $(BIN)/task.o

$(BIN)/main.o: $(KERNEL_SRC)/main.c $(KERNEL_SRC)/graphics.h | $(BIN)
	gcc -m32 -ffreestanding -fno-stack-protector -nostdlib \
	    -fno-pic -fno-pie -fno-asynchronous-unwind-tables \
	    -Wno-write-strings \
	    -I$(KERNEL_SRC) \
	    -c $(KERNEL_SRC)/main.c -o $(BIN)/main.o

$(BIN)/kernel.elf: $(BIN)/kernel_entry.o $(BIN)/final.o $(BIN)/graphics.o $(BIN)/font.o $(BIN)/input.o $(BIN)/graphics_elements.o $(BIN)/task.o $(BIN)/main.o boot/kernel.ld
	ld -m elf_i386 -T boot/kernel.ld \
	    -o $(BIN)/kernel.elf \
	    $(BIN)/kernel_entry.o $(BIN)/final.o $(BIN)/graphics.o $(BIN)/font.o $(BIN)/input.o $(BIN)/graphics_elements.o $(BIN)/task.o $(BIN)/main.o

$(BIN)/kernel.bin: $(BIN)/kernel.elf
	objcopy -O binary $(BIN)/kernel.elf $(BIN)/kernel.bin

run: all
	qemu-system-x86_64 -drive format=raw,file=os.img -m 32M -vga std -no-reboot

clean:
	rm -rf $(BIN) os.img os.img.lock