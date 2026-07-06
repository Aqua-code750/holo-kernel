# HoloKernel

HoloKernel is an experimental bootable kernel skeleton with a simple built-in shell and a user-space prototype `holoshell` that can detect and launch Doom WAD files.

Status
- Kernel multiboot ELF build: requires a Linux toolchain (WSL or native Linux) or an i686-elf cross-toolchain on MSYS2. Building the cross-toolchain on MSYS2 is supported in this repo but can be slow.
- User-space prototype: `user-shell/holoshell` builds and runs on Windows/MSYS2.

Quick actions

Build user-shell (MSYS2 / Windows):
```sh
cd user-shell
make
./holoshell
```

Build kernel (recommended in WSL/Ubuntu):
```sh
sudo apt update
sudo apt install build-essential gcc-multilib nasm grub-pc-bin xorriso qemu-system-x86
cd /mnt/c/Users/kavs1/OneDrive/Desktop/holokernel
make all
make run
```

If you prefer MSYS2, install an `i686-elf` cross-toolchain (or build it from source) before running `make` in the project root.

License: MIT
