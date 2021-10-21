#pragma once

#define CR0_PE 1
#define CR0_PG 31

#define CR4_PAE 5

#define EFER_LME 8

#define SEG_KCODE 1  // kernel code
#define SEG_KDATA 2  // kernel data+stack
#define SEG_NULLASM                                             \
        .word 0, 0;                                             \
        .byte 0, 0, 0, 0

// The 0xC0 means the limit is in 4096-byte units
// and (for executable segments) 32-bit mode.
#define SEG_ASM(type,base,lim)                                  \
        .word (((lim) >> 12) & 0xffff), ((base) & 0xffff);      \
        .byte (((base) >> 16) & 0xff), (0x90 | (type)),         \
                (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)

#define STA_X 0x8 // Executable segment
#define STA_E 0x4 // Expand down (non-executable segments)
#define STA_C 0x4 // Conforming code segment (executable only)
#define STA_W 0x2 // Writeable (non-executable segments)
#define STA_R 0x2 // Readable (executable segments)
#define STA_A 0x1 // Accessed

#define KERNEL_CS 0x8

#define IDT_MAX_VECTORS 256

#define EXCEPTION_ERRCODE_MASK 0x20027d00
#define IDT_HANDLER_SIZE 9

#define KERNEL_ENTRY 0x900
#define E820_MEMORY_MAP_COUNT 0X908
#define E820_MEMORY_MAP_ADDRESS 0X90c
#define KERNEL_VIRTUAL_OFFSET 0xFFFF800000000000

#define VGA_START 0XB8000