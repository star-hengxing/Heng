#pragma once

#include <x86/x86.h>
#include <type.h>

#define IDT_MAX_ENTRIES IDT_MAX_VECTORS

#define IDT_TYPE_TASK 	   0x0C
#define IDT_TYPE_INTERRUPT 0x0E
#define IDT_TYPE_TRAP	   0x0F
#define IDT_RING_0         0x00
#define IDT_RING_1         0x40
#define IDT_RING_2         0x20
#define IDT_RING_3         0x60
#define IDT_PRESENT        0x80

#define IDT_FAULT	 (IDT_PRESENT | IDT_TYPE_INTERRUPT)
#define IDT_EXTERNAL (IDT_PRESENT | IDT_TYPE_INTERRUPT)
#define IDT_TRAP  	 (IDT_PRESENT | IDT_RING_3)

#define IDT_LOAD(idtr) asm volatile("lidt %0" :: "m"(idtr))

typedef struct
{
    u64 rip;
    u64 cs;
    u64 eflags;
    u64 rsp;
    u64 ss;
} interrupt_auto_push;

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
    interrupt_auto_push regs;
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