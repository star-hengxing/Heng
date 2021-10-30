#pragma once

#include <type.h>

#define PAGE_TABLE_MAX_ENTRY 512

#define PAGE_SIZE 4096

#define PAGE_PRESENT  0x1
#define PAGE_WRITE    0x2
#define PAGE_ROOT     0x4
#define PAGE_PWT      0x8
#define PAGE_PCD      0x10
#define PAGE_ACCESSED 0x20
#define PAGE_DIRTY    0x40
#define PAGE_PS       0x80
#define PAGE_GLOBAL   0x100

typedef struct 
{
    u64* table_3;
    u64* table_2;
    u64* table_1;
} page_table_2M;

inline void cr3_load(const void* pt)
{
    asm volatile("mov %%rax, %%cr3" :: "a"(pt));
}

void init_kernel_page_table();