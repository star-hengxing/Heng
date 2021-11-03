#pragma once

#include <type.h>

#define TR_LOAD(tss) asm volatile("ltr %%ax" :: "a"(tss))

typedef struct
{
	u16 limit0;
	u16 base0;
	u8  base1;
	u16 type: 4, zero0: 1, dpl: 2, p: 1, limit1: 4, avl: 1, zero1: 2, G: 1;
	u8  base2;
	u32 base3;
	u32 reserved;
} __attribute__((packed)) TSS_ENTRY;

typedef struct
{
    u32 reserved0;
    u64 RSP0;
    u64 RSP1;
    u64 RSP2;
    u64 reserved1;
    u64 IST1;
    u64 IST2;
    u64 IST3;
    u64 IST4;
    u64 IST5;
    u64 IST6;
    u64 IST7;
    u64 reserved2;
    u16 reserved3;
    u16 IOPB;
} __attribute__((packed)) TSS;