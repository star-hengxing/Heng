#pragma once

#include <type.h>

typedef struct
{
    u16 limit;
    u64 base;
} __attribute__((packed)) SR_80;

inline void interrupt_enable()
{
    asm volatile("sti");
}

inline void interrupt_disable()
{
    asm volatile("cli");
}