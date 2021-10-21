#include <asm/x86.h>
#include <memory/pm.h>
#include <printk.h>
#include <e820.h>

// KERNEL_CODE_END align 4K
extern u8 KERNEL_CODE_END[];

#define ZONE_MAX 2

static zone zone_list[ZONE_MAX];

void init_physical_page()
{
    const u32* map_size = (u32*)(E820_MEMORY_MAP_COUNT + KERNEL_VIRTUAL_OFFSET);
    const e820_map map = 
    {
        *map_size,
        (e820_memory_map*)(E820_MEMORY_MAP_ADDRESS + KERNEL_VIRTUAL_OFFSET)
    };

    uszie zone_index = 0;
    p_page* zone_memory_end = (p_page*)KERNEL_CODE_END;
    for(u32 i = 0; i < map.size; i++)
    {
        if(map.memory[i].type == e820_TYPE_MEMORY)
        {
            zone_list[zone_index].memory = zone_memory_end;
            uszie j = map.memory[i].base;
            for(; j < map.memory[i].base + map.memory[i].size; j += PAGE_SIZE)
            {
                uszie page_index = zone_list[zone_index].count;
                zone_list[zone_index].memory[page_index].base = j;
                zone_list[zone_index].memory[page_index].flag = 0;
                zone_list[zone_index].memory[page_index].ref_count =
                    (j < 0x200000) ? 1 : 0;
                zone_list[zone_index].count++;
            }
            zone_memory_end = &zone_list[zone_index].memory[zone_list[zone_index].count];
            zone_index++;
        }
    }

    for(uszie i = 0; i < ZONE_MAX; i++)
    {
        printk("zone->%d, count->%d physical page\n", i, zone_list[i].count);
        printk("start->0x%x end->0x%x\n", zone_list[i].memory, &zone_list[i].memory[zone_list->count]);
    }
}