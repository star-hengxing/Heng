#include <interrupt/idt.h>
#include <memory/pm.h>
#include <asm/x86.h>
#include <printk.h>
#include <vga.h>
#include <gdt.h>
#include <SR.h>

extern u8 KERNEL_CODE_END[];
extern u8 KERNEL_CODE_START[];
extern u8 TEXT_END[];
extern u8 RODATA_END[];
extern u8 DATA_END[];

void memset(void* desc, i64 value, u64 size);

__attribute__((noreturn))
void main()
{
    memset((void*)DATA_END, 0, KERNEL_CODE_END - DATA_END);
    vga_clear();

    init_gdt();

    init_idt();
    interrupt_enable();

    init_physical_page();

    printk("kernel start->0x%x\n", KERNEL_CODE_START);
    printk("kernel end->0x%x\n\n", KERNEL_CODE_END);

    printk(".text->%d\n", TEXT_END - KERNEL_CODE_START);
    printk(".rodata->0x%x\n", RODATA_END - TEXT_END);
    printk(".data->0x%x\n", DATA_END - RODATA_END);
    printk(".bss->0x%x\n", KERNEL_CODE_END - DATA_END);

    while(1)
    {
        asm volatile("hlt");
    }
}

void memset(void* desc, i64 value, u64 size)
{
    u8* j = (u8*)desc;
    for(u64 i = 0; i < size; i++)
    {
        j[i] = value;
    }
}