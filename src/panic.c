#include <printk.h>
#include <vga.h>
#include <SR.h>

void panic(const char* str)
{
    interrupt_disable();
    vga_clear();
    puts(str);
    asm volatile("hlt");
    while(1);
}