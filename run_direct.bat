@echo off
"C:\Program Files\qemu\qemu-system-x86_64.exe" -cdrom build/holokernel.iso -m 128M -d int -no-reboot -no-shutdown
pause
