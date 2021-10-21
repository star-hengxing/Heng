#include <elf.h>
#include <io.h>

#define SECTOR_SIZE 512

#define ENTER_LONG_MODE_CODE 0x700
#define ENTER_LONG_MODE_CODE_SECTOR 1

#define KERNEL_ENTRY 0x900

#define KERNEL_START_ADDR 0x100000
#define KERNEL_ELF_HEADER_ADDR 0x500
#define KERNEL_ELF_HEADER_SECTOR 2

static inline void wait_disk()
{
    while((inb(0x1f7) & 0xC0) != 0x40);
}

void read_sector(void* dst, u32 section_number)
{
    outb(0x1f2, 1);

    outb(0x1f3, section_number);
    outb(0x1f4, section_number >> 8);
    outb(0x1f5, section_number >> 16);
    outb(0x1f6, (section_number >> 24) | 0xe0);
    outb(0x1f7, 0x20);

    wait_disk();
    /* 循环读取端口数据 */
    insl(0x1f0, dst, 128); // 512 /4
}

void loader()
{
    // main.c
    Elf64_Ehdr* elf = (Elf64_Ehdr*)(KERNEL_ELF_HEADER_ADDR);
    read_sector((void*)elf, KERNEL_ELF_HEADER_SECTOR);
    
    u64* entry = (u64*)KERNEL_ENTRY;
    *entry = (u64)elf->e_entry;

    // Program Headers Address
    Elf64_Phdr* ph = (Elf64_Phdr*)((void*)elf + elf->e_phoff);
    for(u32 i = 0; i < elf->e_phnum; i++)
    {
        // 找到load段，加载.text .rodata .data .bss
        if(ph[i].p_type == PT_LOAD)
        {
            // 先加载elf头里读取的部分代码
            u8* dst = (u8*)KERNEL_START_ADDR;
            const u8* src = (u8*)((void*)elf + ph[i].p_offset);
            const u32 offset = SECTOR_SIZE - ph[i].p_offset;
            for(u32 i = 0; i < offset; i++)
            {
                dst[i] = src[i];
            }
            // entry_64.S
            read_sector((void*)ENTER_LONG_MODE_CODE, ENTER_LONG_MODE_CODE_SECTOR);
            // 加载剩下代码
            u32 start = KERNEL_START_ADDR + offset;
            const u32 end   = KERNEL_START_ADDR + ph[i].p_memsz;
            u32 section_number = KERNEL_ELF_HEADER_SECTOR + 1;
            for(; start < end; start += SECTOR_SIZE, section_number++)
            {
                read_sector((void*)start, section_number);
            }
        }
    }
}