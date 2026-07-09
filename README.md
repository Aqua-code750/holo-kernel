# HoloKernel

A custom, bare-metal x86 kernel designed from scratch to run Doom. 
Developed by Holograph Inc. (ToT)

## Features
- Custom Multiboot Bootloader (GRUB)
- Flat Memory Model (GDT)
- Interrupt Descriptor Table (IDT) with basic ISR/IRQ handlers
- Physical Memory Manager (PMM)
- Virtual Memory Manager (VMM) with hardware paging
- Kernel Heap Allocator (`kmalloc`)
- VESA Framebuffer Graphics (VBE)
- Custom VGA terminal emulator with 8x16 bitmap font
- Custom Virtual File System (Initramfs / VFS) for mounting WAD files

## Doom Engine
HoloKernel incorporates the `doomgeneric` engine port. 
The kernel provides all standard C library functions (`malloc`, `memcpy`, `open`, `read`, etc.) and routes the `doomgeneric` framebuffer directly to the VESA graphics output, allowing DOOM to run perfectly in a bare-metal environment without an underlying operating system!
