@echo off
echo Launching HoloKernel in QEMU...
qemu-system-i386 -kernel build/holokernel.bin -serial stdio
pause
