#include <x86/vga.h>
#include <x86/asm.h>

#include <kernel/printk.h>

void panic(const char* str)
{
    INTERRUPT_DISBALE();
    vga_clear();
    puts(str);
    asm volatile("hlt");
    while(1);
}