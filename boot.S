/* boot.S */
.section .multiboot, "a"
.align 4
.long 0x1BADB002
.long 0x00000007
.long -(0x1BADB002 + 0x00000007)
.long 0
.long 0
.long 0
.long 0
.long 0
.long 0
.long 800
.long 600
.long 32

.section .bss
.align 16
stack_bottom:
.skip 65536
stack_top:

.section .text
.globl _start
_start:
    movl $stack_top, %esp
    pushl %ebx
    pushl %eax
    call kernel_main
    cli
1:
    hlt
    jmp 1b
