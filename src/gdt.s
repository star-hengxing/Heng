#include <gdt.h>

extern void segment_reload();

static GDT_ENTRY gdt[GDT_ENTRIES] = 
{
    { 0 },
    GDT_ENRTY_INIT(0, KERNEL_CODE, 0xfffff),
    GDT_ENRTY_INIT(0, KERNEL_DATA, 0xfffff),
    GDT_ENRTY_INIT(0, USER_CODE,   0xfffff),
    GDT_ENRTY_INIT(0, USER_DATA,   0xfffff)
};

inline void init_gdt()
{
    const SR_80 idtr =
    {
        (u16)sizeof(GDT_ENTRY) * GDT_ENTRIES - 1,
        (u64)&gdt[0]
    };

    gdt_load(&idtr);

    segment_reload();
}