#include <kernel/printk.h>
#include <tool/stdarg.h>
#include <x86/vga.h>

void puts(const char* str)
{
    for(u64 i = 0; str[i]; i++)
    {
        vga_putchar((u8)str[i]);
    }
}

static void itoa(u64 n, u8 base)
{
    static const char* digits = "0123456789abcdef";

    char buffer[33];
	char* p = buffer + 32;
    *p = '\0';

    do
    {
		p--;
        *p = digits[n % base];
        n /= base;
    }while(n);
    puts(p);
}

void printk(const char* format, ...)
{
    va_list args;
	va_start(args, format);

    const char* c = format;
    for(; *c; c++)
    {
        if(*c != '%')
        {
            vga_putchar(*c);
            continue;
        }

        c++;
        switch(*c)
        {
            case 'c':
                vga_putchar((u8)va_arg(args, i32));
                break;
            case 's':
                puts(va_arg(args, const char*));
                break;
            case 'd':
                itoa((i64)va_arg(args, i32), 10);
                break;
            case 'x':
                itoa((u64)va_arg(args, i32), 16);
                break;                                
            case '%':
                vga_putchar('%');
                break;
        
            default:
                puts("invaild char.\n");
                break;
        }
    }

    va_end(args);
}