# pragma once

#include <type.h>

// 老设备io慢，cpu等一下io
static inline void io_delay()
{
    asm volatile("jmp 1f; 1: jmp 1f; 1:");
}

static inline u8 inb(u16 port)
{
    u8 data;
    asm volatile("inb %1, %0" : "=a"(data) : "d"(port));
    return data;
}

static inline void outb(u16 port, u8 data)
{
    asm volatile("outb %0, %1" :: "a"(data), "d"(port));
}

static inline void insl(u16 port, void* dst, u32 count)
{
    asm volatile
    (
        "cld;"
        "repne insl;"
        : "=D"(dst), "=c"(count)
        : "d"(port), "0"(dst), "1"(count)
        : "memory", "cc"
    );
}