@echo off
echo Launching HoloKernel in QEMU...
qemu-system-x86_64 -kernel build/holokernel.bin -serial stdio
pause
