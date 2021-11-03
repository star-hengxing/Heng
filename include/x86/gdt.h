#pragma once

#include <type.h>

#define GDT_TYPE_LDT       0x0002

#define GDT_TYPE_VAILD_TSS 0x0009
#define GDT_TYPE_BUSSY_TSS 0x000B

#define GDT_TYPE_INTERRUPT 0x000E
#define GDT_TYPE_TRAP      0x000F
#define GDT_TYPE_CALL      0x000C

#define GDT_PRESENT        0x0080

#define GDT_RING_0         0x0000
#define GDT_RING_1         0x0020
#define GDT_RING_2         0x0040
#define GDT_RING_3         0x0060

#define GDT_CODE_D         0x4000
#define GDT_CODE_L         0x2000
#define GDT_CODE_C         0x0004

// AMD64 Manual Volume 2 4.8
#define GDT_CODE(code) (code | GDT_CODE_L | 0x18)
#define GDT_DATA(data) (data | 0x12)

#define GDT_MAX_ENTRIES 7

#define GDT_KERNEL_CODE GDT_CODE(GDT_PRESENT | GDT_RING_0)
#define GDT_KERNEL_DATA GDT_DATA(GDT_PRESENT | GDT_RING_0)

#define GDT_USER_CODE   GDT_CODE(GDT_PRESENT | GDT_RING_3)
#define GDT_USER_DATA   GDT_DATA(GDT_PRESENT | GDT_RING_3)

#define GDTR_LOAD(gdtr) asm volatile("lgdt %0" :: "m"(gdtr))

/* 8 byte segment descriptor */
typedef struct
{
	u16	limit0;
	u16	base0;
	u16	base1: 8, type: 4, s: 1, dpl: 2, p: 1;
	u16	limit1: 4, avl: 1, l: 1, d: 1, g: 1, base2: 8;
} __attribute__((packed)) GDT_ENTRY;

#define GDT_ENRTY_INIT(base, flag, limit) \
    {                                     \
        .limit0 = limit & 0xFF,           \
        .base0  = base & 0xFF,            \
        .base1  = (base >> 16) & 0xF,     \
        .type   = flag & 0xF,             \
        .s      = (flag >> 4) & 0x1,      \
        .dpl    = (flag >> 5) & 0x3,      \
        .p      = (flag >> 7) & 0x1,      \
        .limit1 = (limit >> 16) & 0xF,    \
        .avl    = (flag >> 12) & 0x1,     \
        .l      = (flag >> 13) & 0x1,     \
        .d      = (flag >> 14) & 0x1,     \
        .g      = (flag >> 15) & 0x1,     \
        .base2  = (base >> 24) & 0xF,     \
    }

void init_gdt();