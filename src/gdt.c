#include <x86/tss.h>
#include <x86/gdt.h>
#include <x86/SR.h>

#define TSS_MAX_SIZE sizeof(TSS) - 1
#define TSS_INDEX 5

inline static void set_tss_entry(TSS_ENTRY* entry, u64 base, u16 flag, u32 limit)
{
    entry->limit0   = limit & 0xFF;
    entry->base0    = base & 0xFF;
    entry->base1    = (base >> 16) & 0xF;
    entry->type     = flag & 0xF;  
    entry->zero0    = 0;      
    entry->dpl      = (flag >> 5) & 0x3;
    entry->p        = (flag >> 7) & 0x1;
    entry->limit1   = (limit >> 16) & 0xF;
    entry->avl      = (flag >> 12) & 0x1;
    entry->zero1    = 0;
    entry->G        = (flag >> 15) & 0x1;
    entry->base2    = (base >> 24) & 0xF;
    entry->base3    = (base >> 32) & 0xFFFF;
    entry->reserved = 0;
}

inline static void set_tss(TSS* tss, u64 rsp0, u64 rsp1, u64 rsp2, u64 ist1, u64 ist2,
                           u64 ist3, u64 ist4,u64 ist5, u64 ist6, u64 ist7, u16 iopb)
{
    tss->RSP0 = rsp0;
    tss->RSP1 = rsp1;
    tss->RSP2 = rsp2;
    tss->IST1 = ist1;
    tss->IST2 = ist2;
    tss->IST3 = ist3;
    tss->IST4 = ist4;
    tss->IST5 = ist5;
    tss->IST6 = ist6;
    tss->IST7 = ist7;

    tss->reserved0 = 0;
    tss->reserved1 = 0;
    tss->reserved2 = 0;
    tss->reserved3 = 0;

    tss->IOPB = iopb;
}

static GDT_ENTRY gdt[GDT_MAX_ENTRIES] = 
{
    { 0 },
    GDT_ENRTY_INIT(0, GDT_KERNEL_CODE, 0),
    GDT_ENRTY_INIT(0, GDT_KERNEL_DATA, 0),
    GDT_ENRTY_INIT(0, GDT_USER_CODE,   0),
    GDT_ENRTY_INIT(0, GDT_USER_DATA,   0)
};

TSS tss;

void init_gdt()
{
    set_tss_entry((TSS_ENTRY*)(&gdt[TSS_INDEX]), (u64)(&tss), (GDT_PRESENT | GDT_TYPE_VAILD_TSS), TSS_MAX_SIZE);
    
    set_tss(&tss, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    const SR_80 idtr =
    {
        (u16)sizeof(GDT_ENTRY) * GDT_MAX_ENTRIES - 1,
        (u64)gdt
    };

    GDTR_LOAD(idtr);
    TR_LOAD(sizeof(GDT_ENTRY) * TSS_INDEX);

    asm volatile
    (
        "lea init_gdt_jmp(%rip), %rax\n"
        "pushq $0x8\n"
        "pushq %rax\n"
        "lretq\n"
        "init_gdt_jmp:\n"
        "movl $0x10, %eax\n"
        "movl %eax, %ds\n"
        "movl %eax, %ss\n"
        "movl %eax, %es\n"
        "movl %eax, %fs\n"
        "movl %eax, %gs\n"
    );
}