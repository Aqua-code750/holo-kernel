/* boot.S */
.section .multiboot
.align 4
.long 0x1BADB002
.long 0x00000003
.long -(0x1BADB002 + 0x00000003)

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
