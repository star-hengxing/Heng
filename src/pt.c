#include <tool/assert.h>
#include <x86/x86.h>
#include <x86/pt.h>

#define OFFSET(addr) ((void*)addr - KERNEL_VIRTUAL_OFFSET)

#define KERNEL_PAGE_TABLE_DIRECTORY(addr) ((u64)OFFSET(addr) | (PAGE_PRESENT | PAGE_WRITE))

#define KERNEL_PAGE_TABLE_ATTRIBUTE(addr) (addr | PAGE_PRESENT | PAGE_WRITE | PAGE_PS | PAGE_GLOBAL)

#define SET_KERNEL_PAGE_TABLE(table, level, entry, value) table.table_##level[entry] = (u64)value

extern u8 KERNEL_CODE_END[];

static page_table_2M kernel_page_table = 
{
    (u64*)KERNEL_CODE_END,
    (u64*)(KERNEL_CODE_END + PAGE_SIZE),
    (u64*)(KERNEL_CODE_END + PAGE_SIZE * 2)
};

u64 kernel_virtual_memory_map(usize entry, usize p_addr)
{
    assert(entry < PAGE_TABLE_MAX_ENTRY);
    SET_KERNEL_PAGE_TABLE(kernel_page_table, 1, entry, KERNEL_PAGE_TABLE_ATTRIBUTE(p_addr));
    return KERNEL_VIRTUAL_OFFSET + entry * PAGE_SIZE * PAGE_TABLE_MAX_ENTRY;
}

void init_kernel_page_table()
{
    SET_KERNEL_PAGE_TABLE(kernel_page_table, 3, PAGE_TABLE_MAX_ENTRY / 2,
                          KERNEL_PAGE_TABLE_DIRECTORY(&kernel_page_table.table_2[0]));
    SET_KERNEL_PAGE_TABLE(kernel_page_table, 2, 0,
                          KERNEL_PAGE_TABLE_DIRECTORY(&kernel_page_table.table_1[0]));
    SET_KERNEL_PAGE_TABLE(kernel_page_table, 1, 0, KERNEL_PAGE_TABLE_ATTRIBUTE(0));

    cr3_load(OFFSET(kernel_page_table.table_3));
}