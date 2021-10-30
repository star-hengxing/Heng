#pragma once

#include <type.h>

// physical page
typedef struct
{
    u64 flag: 8, ref_count: 8, base: 48;
} p_page;

typedef struct
{
    p_page* memory;
    u64 count;
    // struct zone* next;
} zone;

typedef struct 
{
    p_page* front;
    p_page* tail;
} page_map;

void init_physical_page();

p_page* p_page_alloc();

p_page* p_pages_alloc(usize);

void p_page_free(p_page*);

void p_pages_free(p_page*, usize);