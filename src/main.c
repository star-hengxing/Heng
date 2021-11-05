#include <x86/x86.h>
#include <x86/vga.h>
#include <x86/gdt.h>
#include <x86/asm.h>
#include <x86/pt.h>

#include <interrupt/idt.h>
#include <memory/pm.h>

#include <lib/string.h>

#include <kernel/printk.h>
#include <kernel/thread.h>

#include <keyboard.h>

extern u8 KERNEL_CODE_END[];
extern u8 KERNEL_CODE_START[];
extern u8 TEXT_END[];
extern u8 RODATA_END[];
extern u8 DATA_END[];

__attribute__((noreturn))
void main()
{
    memset((void*)DATA_END, 0, KERNEL_CODE_END - DATA_END);
    vga_clear();

    init_kernel_page_table();
    
    init_gdt();
    init_idt();
    INTERRUPT_ENABLE();

    printk("kernel code: 0x%x-0x%x\n", KERNEL_CODE_START, KERNEL_CODE_END);
    printk("kernel page table: 0x%x-0x%x\n", KERNEL_CODE_END, KERNEL_CODE_END + PAGE_SIZE * 3);

    init_physical_page();

    init_keyboard();
    // init_kernel_thread();

    // while(1)
    // {
    //     INTERRUPT_DISBALE();
    //     printk("Main_Thread ");
    //     INTERRUPT_ENABLE();
    // }

    while(1)
    {
        asm volatile("hlt");
    }
}