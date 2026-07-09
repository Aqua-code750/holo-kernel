@echo off
echo Launching HoloKernel from ISO in QEMU...
"C:\Program Files\qemu\qemu-system-x86_64.exe" -cdrom build/holokernel.iso -m 128M -serial stdio
pause
