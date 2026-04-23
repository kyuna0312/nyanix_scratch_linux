# Nyanix Unified Build System
# Supports: x86, x86_64, ARM64

.PHONY: all build-x64 build-arm64 clean run-x64 run-arm run

all: build-x64 build-arm64

build-x64:
	$(MAKE) -C boot-x64 clean
	$(MAKE) -C boot-x64

build-arm64:
	$(MAKE) -C boot-arm64 clean
	$(MAKE) -C boot-arm64

clean:
	$(MAKE) -C boot clean || true
	$(MAKE) -C boot-x64 clean
	$(MAKE) -C boot-arm64 clean

run-x64: build-x64
	qemu-system-x86_64 -m 256 -kernel boot-x64/bin/kernel.elf -nographic

run-arm: build-arm64
	qemu-system-aarch64 -M virt -m 256 -cpu cortex-a57 -kernel boot-arm64/bin/kernel.elf -nographic

run: run-x64