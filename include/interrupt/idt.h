#pragma once

#include <asm/x86.h>
#include <type.h>
#include <SR.h>

#define IDT_MAX_ENTRIES IDT_MAX_VECTORS

#define IDT_DESCRIPTOR_16_INTERRUPT 0x06
#define IDT_DESCRIPTOR_16_TRAP 	    0x07
#define IDT_DESCRIPTOR_32_TASK 	    0x05
#define IDT_DESCRIPTOR_32_INTERRUPT 0x0E
#define IDT_DESCRIPTOR_32_TRAP		0x0F
#define IDT_DESCRIPTOR_RING1  		0x40
#define IDT_DESCRIPTOR_RING2  		0x20
#define IDT_DESCRIPTOR_RING3  		0x60
#define IDT_DESCRIPTOR_PRESENT		0x80

#define IDT_DESCRIPTOR_FAULT	(IDT_DESCRIPTOR_32_INTERRUPT | IDT_DESCRIPTOR_PRESENT)
#define IDT_DESCRIPTOR_EXTERNAL (IDT_DESCRIPTOR_32_INTERRUPT | IDT_DESCRIPTOR_PRESENT)
#define IDT_DESCRIPTOR_TRAP  	(IDT_DESCRIPTOR_32_INTERRUPT | IDT_DESCRIPTOR_PRESENT | IDT_DESCRIPTOR_RING3)

typedef struct
{
    u64 rax;
    u64 rbx;
    u64 rcx;
    u64 rdx;
    u64 rsi;
    u64 rdi;
    u64 rbp;
    u64 r8;
    u64 r9;
    u64 r10;
    u64 r11;
    u64 r12;
    u64 r13;
    u64 r14;
    u64 r15;
    u64 interrupt_code;
    u64 error_code; // 错误代码(有中断错误代码的中断会由CPU压入)
    // 以下由处理器自动压入
    u64 rip;
    u64 cs;
    u64 eflags;
    u64 rsp;
    u64 ss;
} interrupt_regs;

typedef struct
{
	u16 base0;
	u16 kernel_cs;
	u8  ist: 3, zero: 5;
	u8  type: 4, z: 1, dpl: 2, p: 1;
	u16 base1;
	u32 base2;
	u32 reserved;   // Set to zero
} __attribute__((packed)) IDT_ENTRY;

typedef void (*idt_handle)(interrupt_regs* stack);

typedef struct
{
	u64 count;
	idt_handle handler;
} IDT;

void interrupt_count();

void init_idt();

void interrupt_handler_register(u8 vector, idt_handle fn);

inline void idt_load(const SR_80* idtr)
{
	asm volatile("lidt %0" :: "m"(*idtr));
}