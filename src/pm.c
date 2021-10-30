#include <x86/x86.h>
#include <x86/e820.h>
#include <x86/pt.h>

#include <memory/pm.h>

#include <kernel/printk.h>
#include <kernel/panic.h>

#include <tool/assert.h>

// KERNEL_CODE_END align 4K
extern u8 KERNEL_CODE_END[];

#define ZONE_MAX 2

static zone zone_list[ZONE_MAX];

static page_map view;

static void init_phsical_page_alloctor()
{
    view.front = &zone_list[1].memory[512];
    view.tail  = &zone_list[1].memory[zone_list[1].count];
}

static inline bool is_free_page(const p_page* page)
{
    return page->ref_count == 0;
}

p_page* p_page_alloc()
{
    p_page* temp = view.front;
    temp->ref_count++;
    do
    {
        if(is_free_page(view.front))
        {
            return temp;
        }
        view.front++;
    } while(view.front < view.tail);
    panic("no free memory!");
}

p_page* p_pages_alloc(usize size)
{
    view.tail -= size;
    p_page* temp = view.tail;

    for(usize i = 0; i < size; i++)
    {
        temp[i].ref_count++;
    }

    do
    {
        if(is_free_page(view.front))
        {
            return temp;
        }
        view.tail--;
    } while(view.front < view.tail);
    panic("no free memory!");
}

void p_page_free(p_page* page)
{
    page->ref_count--;
    if(page->ref_count == 0 && page < view.front)
    {
        view.front = page;
    }
    assert(view.front >= zone_list[1].memory);
}

void p_pages_free(p_page* page, usize size)
{
    bool flag = true;
    for(usize i = 0; i < size; i++)
    {
        page[i].ref_count--;
        if(page[i].ref_count != 0)
        {
            flag = false;
        }
    }
    // if page ref all zero
    view.tail += (flag == true ? size : 0);
    assert(view.tail <= &zone_list[1].memory[zone_list[1].count]);
}

void init_physical_page()
{
    const u32* map_size = (u32*)(E820_MEMORY_MAP_COUNT + KERNEL_VIRTUAL_OFFSET);
    const e820_map map  =
    {
        *map_size,
        (e820_memory_map*)(E820_MEMORY_MAP_ADDRESS + KERNEL_VIRTUAL_OFFSET)
    };

    usize zone_index        = 0;
    p_page* zone_memory_end = (p_page*)((void*)KERNEL_CODE_END + PAGE_SIZE * 3);
    for(u32 i = 0; i < map.size; i++)
    {
        if(map.memory[i].type == e820_TYPE_MEMORY)
        {
            zone_list[zone_index].memory = zone_memory_end;
            usize j                      = map.memory[i].base;
            for(; j < map.memory[i].base + map.memory[i].size; j += PAGE_SIZE)
            {
                usize page_index                                   = zone_list[zone_index].count;
                zone_list[zone_index].memory[page_index].base      = j;
                zone_list[zone_index].memory[page_index].flag      = 0;
                zone_list[zone_index].memory[page_index].ref_count = (j < 0x200000) ? 1 : 0;
                zone_list[zone_index].count++;
            }
            zone_memory_end = &zone_list[zone_index].memory[zone_list[zone_index].count];
            zone_index++;
        }
    }

    init_phsical_page_alloctor();

    for(usize i = 0; i < ZONE_MAX; i++)
    {
        printk("zone = %d, count = %d physical page\n", i, zone_list[i].count);
        printk("start = 0x%x end = 0x%x\n", zone_list[i].memory,
               &zone_list[i].memory[zone_list[i].count]);
    }
}