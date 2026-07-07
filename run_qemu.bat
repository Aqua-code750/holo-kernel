@echo off
echo Launching HoloKernel in QEMU...
"C:\Program Files\qemu\qemu-system-x86_64.exe" -kernel build/holokernel.bin -serial stdio
pause
