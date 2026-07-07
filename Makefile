CC ?= gcc
CFLAGS ?= -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-builtin -nostdlib -O2 -Wall -Wextra
AS ?= nasm
LDFLAGS ?= -nostdlib -static -T linker.ld -no-pie

all: build/holokernel.bin build/holokernel.iso

build:
	mkdir -p build

boot.o: boot.S
	$(CC) $(CFLAGS) -c $< -o $@

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

interrupts.o: interrupts.S
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure build directory exists (order-only) and link with compiler driver
build/holokernel.bin: boot.o kernel.o interrupts.o | build
	$(CC) $(LDFLAGS) -o $@ $^

build/holokernel.iso: build/holokernel.bin grub.cfg
	mkdir -p build/iso/boot/grub
	cp build/holokernel.bin build/iso/boot/holokernel.bin
	cp grub.cfg build/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ build/iso > /dev/null 2>&1

run: build/holokernel.iso
	qemu-system-x86_64 -cdrom build/holokernel.iso -serial stdio

clean:
	rm -rf build

.PHONY: all run clean
