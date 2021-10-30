#include <x86/x86.h>

.globl idt_handler_array
.globl exception_handler
idt_handler_array:
    i = 0
    .rept IDT_MAX_VECTORS // 256
    .if i >= 32
        push $0
    .else
        .if ((EXCEPTION_ERRCODE_MASK >> i)) & 1 == 0
            push $0
        .endif
    .endif
    push $i
    jmp idt_handler_common
    i = i + 1
    .if i < 32
        .fill idt_handler_array + i * IDT_HANDLER_SIZE - ., 1, 0xcc
    .endif
    .endr

idt_handler_common:
    push %r15
    push %r14
    push %r13
    push %r12
    push %r11
    push %r10
    push %r9
    push %r8
    push %rbp
    push %rdi
    push %rsi
    push %rdx
    push %rcx
    push %rbx
    push %rax

    mov %rsp, %rdi
    call exception_handler
    
    pop %rax
    pop %rbx
    pop %rcx
    pop %rdx
    pop %rsi
    pop %rdi
    pop %rbp
    pop %r8
    pop %r9
    pop %r10
    pop %r11
    pop %r12
    pop %r13
    pop %r14
    pop %r15

    add $16, %rsp
    iretq
