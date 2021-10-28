#include <asm/x86.h>
#include <printk.h>
#include <pt.h>

#define OFFSET(addr) ((void*)addr - KERNEL_VIRTUAL_OFFSET)

#define KERNEL_PAGE_TABLE(addr) ((u64)OFFSET(addr) | (PAGE_PRESENT | PAGE_WRITE))

#define print(expr) printk("%s: 0x%x\n", #expr, expr)

#define kernel_page_table_set(table, level, entry, value) \
    table.table_##level[entry] = (u64)value

extern u8 KERNEL_CODE_END[];

static page_table_2M kernel_page_table = 
{
    (u64*)KERNEL_CODE_END,
    (u64*)(KERNEL_CODE_END + PAGE_SIZE),
    (u64*)(KERNEL_CODE_END + PAGE_SIZE * 2)
};

void init_kernel_page_table()
{
    kernel_page_table_set(kernel_page_table, 3, 256, KERNEL_PAGE_TABLE(&kernel_page_table.table_2[0]));
    kernel_page_table_set(kernel_page_table, 2, 0, KERNEL_PAGE_TABLE(&kernel_page_table.table_1[0]));
    kernel_page_table_set(kernel_page_table, 1, 0, (PAGE_PRESENT | PAGE_WRITE | PAGE_PS));
    
    print(kernel_page_table.table_3[256]);
    print(kernel_page_table.table_2[0]);
    print(kernel_page_table.table_1[0]);

    cr3_load(OFFSET(kernel_page_table.table_3));
}