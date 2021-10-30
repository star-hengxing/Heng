#include <x86/x86.h>
// .org: 0x700
.globl entry_64
.code32
entry_64:
    mov $0x80000000, %eax
    cpuid
    cmp $0x80000001, %eax
    jb no_long_mode

    mov $0x80000001, %eax
    cpuid
    bts $29, %edx
    jz no_long_mode

    lgdt gdtr_entry_64
    // memset(edi, eax, ecx)
    mov $0x1000, %edi
    xor %eax, %eax
    mov $0x4000, %ecx
    rep stosb
    // 初始化四级页表
    mov $0x2003, %eax
    mov %eax, 0x1000
    mov %eax, 0x1800
    mov $0x3003, %eax
    mov %eax, 0x2000
    mov $0x4003, %eax
    mov %eax, 0x3000

    mov $0x03, %eax
    mov $0x4000, %ebx
page_table_init:
    mov %eax, (%ebx)
    add $0x1000, %eax
    add $0x8, %ebx
    cmp $0x5000, %ebx
    jnz page_table_init

enable_long_mode:
    mov $0x1000, %eax
    mov %eax, %cr3

    mov %cr4, %eax
    bts $CR4_PAE, %eax
    mov %eax, %cr4

    mov $0xC0000080, %ecx
    rdmsr
    bts $EFER_LME, %eax
    wrmsr

    mov %cr0, %eax
    bts $CR0_PG, %eax
    mov %eax, %cr0

    mov $long_mode, %eax
    push $KERNEL_CS
    push %eax
    lret

no_long_mode:
    hlt
    jmp no_long_mode

.code64
long_mode:
    movl $0x10, %eax
    movl %eax, %ds
    movl %eax, %ss
    movl %eax, %es
    movl %eax, %fs
    movl %eax, %gs
    // temp stack
    movq $KERNEL_VIRTUAL_OFFSET, %rax
    addq %rax, %rsp
    movq KERNEL_ENTRY, %rax
    jmp *%rax

gdt_entry_64:
    .quad    0x0000000000000000
    .quad    0x0020980000000000    /* KERNEL_CODE */
    .quad    0x000092000000ffff    /* KERNEL_DATA */

gdtr_entry_64:
    .word    gdtr_entry_64 - gdt_entry_64 - 1
    .long    gdt_entry_64
