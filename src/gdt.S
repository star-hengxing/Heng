#include <asm/x86.h>

.globl init_gdt
.code64
init_gdt:
    lea init_gdt_jmp(%rip), %rax
    pushq $KERNEL_CS
    pushq %rax
    lgdt gdtr_64(%rip)
    lretq

init_gdt_jmp:
    movl $0x10, %eax
    movl %eax, %ds
    movl %eax, %ss
    movl %eax, %es
    movl %eax, %fs
    movl %eax, %gs
    ret

.data
gdt_64:
    .quad    0x0000000000000000
    .quad    0x0020980000000000    // KERNEL_CODE_64
    .quad    0x0000920000000000    // KERNEL_DATA_64
    .quad    0x0020F80000000000    // USER_CODE_64
    .quad    0x0000F20000000000    // USER_DATA_64

gdtr_64:
    .word    gdtr_64 - gdt_64 - 1
    .quad    gdt_64
