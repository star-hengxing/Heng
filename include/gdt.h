#pragma once

#include <type.h>
#include <SR.h>

#define GDT_ENTRIES 5

#define GDT_PRESENT 0x0080

#define GDT_RING_0 0x0000
#define GDT_RING_1 0x0020
#define GDT_RING_2 0x0040
#define GDT_RING_3 0x0060

#define GDT_CODE_D 0x4000
#define GDT_CODE_L 0x2000
#define GDT_CODE_C 0x0004

#define KERNEL_CODE (GDT_CODE_L | GDT_PRESENT | GDT_RING_0)
#define KERNEL_DATA (GDT_PRESENT | GDT_RING_0)
#define USER_CODE   (GDT_CODE_L | GDT_PRESENT | GDT_RING_3)
#define USER_DATA   (GDT_PRESENT | GDT_RING_3)

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

inline void gdt_load(const SR_80* gdtr)
{
	asm volatile("lgdt %0" :: "m"(*gdtr));
}