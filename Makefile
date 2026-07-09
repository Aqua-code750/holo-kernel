CC ?= gcc
CFLAGS ?= -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-builtin -nostdlib -O2 -Wall -Wextra
AS ?= nasm
LDFLAGS ?= -m32 -nostdlib -static -T linker.ld -no-pie

all: build/holokernel.bin build/holokernel.iso

build:
	mkdir -p build

boot.o: boot.S
	$(CC) $(CFLAGS) -c $< -o $@

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

interrupts.o: interrupts.S
	$(CC) $(CFLAGS) -c $< -o $@

gdt.o: gdt.c
	$(CC) $(CFLAGS) -c $< -o $@

gdt_flush.o: gdt_flush.S
	$(CC) $(CFLAGS) -c $< -o $@

pmm.o: pmm.c
	$(CC) $(CFLAGS) -c $< -o $@

vmm.o: vmm.c
	$(CC) $(CFLAGS) -c $< -o $@

kheap.o: kheap.c
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure build directory exists (order-only) and link with compiler driver
build/holokernel.bin: boot.o kernel.o interrupts.o gdt.o gdt_flush.o pmm.o vmm.o kheap.o | build
	$(CC) $(LDFLAGS) -o $@ $^

DOOM1.WAD:
	curl -k -L -o DOOM1.WAD "https://distro.ibiblio.org/slitaz/sources/packages/d/doom1.wad"

build/holokernel.iso: build/holokernel.bin grub.cfg DOOM1.WAD
	mkdir -p build/iso/boot/grub
	cp build/holokernel.bin build/iso/boot/holokernel.bin
	cp DOOM1.WAD build/iso/boot/DOOM1.WAD
	cp grub.cfg build/iso/boot/grub/grub.cfg
	grub-mkrescue -o build/holokernel.iso build/iso > /dev/null 2>&1

run: build/holokernel.iso
	qemu-system-x86_64 -cdrom build/holokernel.iso -serial stdio

clean:
	rm -rf build

.PHONY: all run clean
