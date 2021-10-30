#include <x86/x86.h>
#include <x86/vga.h>

static vga* screen          = (vga*)(VGA_START + KERNEL_VIRTUAL_OFFSET);
static const u32 VGA_WIDTH  = 80;
static const u32 VGA_HEIGHT = 25;
static u32 VGA_X            = 0;
static u32 VGA_Y            = 0;

inline u8 vga_entry_color(vga_color fg, vga_color bg)
{
    return fg | bg << 4;
}

void vga_clear()
{
    const vga c = {' ', vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK)};
    VGA_X = VGA_Y = 0;
    for(u32 i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
    {
        screen[i] = c;
    }
}

inline u32 vga_index(u32 x, u32 y)
{
    return y * VGA_WIDTH + x;
}

void vga_scroll()
{
    for(u32 i = VGA_WIDTH; i < VGA_WIDTH * VGA_HEIGHT; i++)
    {
        screen[i - 80] = screen[i];
    }
}

void vga_putchar(u8 c)
{
    if(VGA_Y > VGA_HEIGHT)
    {
        vga_scroll();
        VGA_Y = 24;
    }

    if(c == '\n' || VGA_X > VGA_WIDTH)
    {
        VGA_Y++;
        VGA_X = 0;

        if(c == '\n')
            return;
    }

    screen[vga_index(VGA_X, VGA_Y)].c = c;
    VGA_X++;
}