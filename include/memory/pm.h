#pragma once

#include <type.h>

#define PAGE_PRESENT 0x1
/** 
 * R/W -- 位 1 是读 / 写 (Read/Write) 标志。如果等于 1 ，表示页面可以被读、写或执行。
 * 如果为 0 ，表示页面只读或可执行。
 * 当处理器运行在超级用户特权级 （级别 0,1 或 2） 时，则 R/W 位不起作用。
 * 页目录项中的 R/W 位对其所映射的所有页面起作用。
 */
#define PAGE_WRITE 0x2

#define PAGE_SIZE 4096

// physical page
typedef struct
{
    u8 flag;
    u8 ref_count;
    u64 base;
} p_page;

typedef struct
{
    p_page* memory;
    u64 count;
    // struct zone* next;
} zone;

void init_physical_page();