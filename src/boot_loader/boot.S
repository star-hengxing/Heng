#include <x86/x86.h>

.globl real_mode
.code16
real_mode:
	xorw %ax, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss

	mov $E820_MEMORY_MAP_ADDRESS, %di
	xor %ebx, %ebx
	mov $20, %ecx
	mov $0x0534d4150, %edx
e820_int:
	mov $0xe820, %eax
	int $0x15

	add $20, %di
	mov E820_MEMORY_MAP_COUNT, %eax
	inc %eax
	mov %eax, E820_MEMORY_MAP_COUNT
	
	test %ebx, %ebx
	jnz e820_int
	# fast A20
	in $0x92, %al
	or $0x2,  %al
	out %al, $0x92
	
	lgdt gdtr_32
	cli
	
	mov %cr0, %eax
	or  $CR0_PE, %eax
	mov %eax, %cr0
	
	# 清空16bit指令的流水线，执行32位代码
	ljmp $KERNEL_CS, $protected_mode

.code32
protected_mode:
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	
	movl $0x7c00, %esp
	call loader

    jmp	0x700

.p2align 2                                  # force 4 byte alignment
gdt_32:
  	SEG_NULLASM                             # null seg
  	SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)   # code seg
  	SEG_ASM(STA_W, 0x0, 0xffffffff)         # data seg

gdtr_32:
  	.word gdtr_32 - gdt_32 - 1
  	.long gdt_32
