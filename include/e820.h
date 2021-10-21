#pragma once

#include <type.h>

#define e820_TYPE_MEMORY 1
#define e820_TYPE_RESERVED 2

typedef struct
{
    u64 base;
    u64 size;
    u32 type;
} __attribute__((packed)) e820_memory_map;

typedef struct
{
    u32 size;
    e820_memory_map* memory;
} __attribute__((packed)) e820_map;