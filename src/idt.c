#include <interrupt/8259.h>
#include <interrupt/idt.h>
#include <printk.h>
#include <io.h>

static IDT_ENTRY idt_entries[IDT_MAX_ENTRIES];

static IDT idt[IDT_MAX_ENTRIES];

extern u8 idt_handler_array[];

static void idt_set(u8 number, u64 base, u8 flag, u8 ist)
{
    idt_entries[number].base0     = base & 0xFFFF;
    idt_entries[number].kernel_cs = KERNEL_CS;
    idt_entries[number].ist       = ist;
    idt_entries[number].zero      = 0;
    idt_entries[number].type      = flag & 0xF;
    idt_entries[number].z         = (flag >> 4) & 0x1;
    idt_entries[number].dpl       = (flag >> 5) & 0x3;
    idt_entries[number].p         = (flag >> 7) & 0x1;
    idt_entries[number].base1     = (base >> 16) & 0xFFFF;
    idt_entries[number].base2     = (base >> 32) & 0xFFFFFFFF;
    idt_entries[number].reserved  = 0;
}

void interrupt_count()
{
    for(u8 i = 0; i < IDT_MAX_ENTRIES; i++)
    {
        if(idt[i].count != 0)
        {
            printk("%d: %d\n", i, idt[i].count);
        }
    }
}

static inline void pic_eoi(interrupt_regs* stack)
{
    if(stack->interrupt_code >= 40)
    {
		outb(PIC_SLAVE_COMMOND, PIC_EOI);
    }
	outb(PIC_MASTER_COMMOND, PIC_EOI);
}

static void pic_remap()
{
    // 初始化主片、从片
    outb(PIC_MASTER_COMMOND, ICW1_INIT | ICW1_ICW4);
    outb(PIC_SLAVE_COMMOND,  ICW1_INIT | ICW1_ICW4);
    // 设置主片 IRQ 从 0x20(32) 号中断开始
    outb(PIC_MASTER_DATA, 0x20);
    // 设置从片 IRQ 从 0x28(40) 号中断开始
    outb(PIC_SLAVE_DATA, 0x28);
    // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(PIC_MASTER_DATA, 4);
    // ICW3: tell Slave PIC its cascade identity (0000 0010)
    outb(PIC_SLAVE_DATA, 2);
    // 设置主片和从片按照 8086 的方式工作
    outb(PIC_MASTER_DATA, ICW4_8086);
    outb(PIC_SLAVE_DATA,  ICW4_8086);
    // 设置主从片允许中断
    // outb(PIC_MASTER_DATA, 0);
    // outb(PIC_SLAVE_DATA,  0);
}

void exception_handler(interrupt_regs* stack)
{
    pic_eoi(stack);

    idt[stack->interrupt_code].count++;
    if(idt[stack->interrupt_code].handler)
    {
        idt[stack->interrupt_code].handler(stack);
    }
    else
    {
        printk("[rip->0x%x, int->%d, error code->%d]\n",
            stack->rip, stack->interrupt_code, stack->error_code);
    }
}

inline void interrupt_handler_register(u8 vector, idt_handle fn)
{
    if(idt[vector].handler)
    {
        printk("Interruption %d has been registered\n", vector);
    }
    else
    {
        idt[vector].handler = fn;
    }
}

void timer_callback(interrupt_regs* stack)
{
    printk("hello world!\n");
}

void init_timer(u32 frequency)
{
    // 注册时间相关的处理函数
    interrupt_handler_register(32, timer_callback);

    // Intel 8253/8254 PIT芯片 I/O端口地址范围是40h~43h
    // 输入频率为 1193180，frequency 即每秒中断次数
    u32 divisor = 1193180 / frequency;

    // D7 D6 D5 D4 D3 D2 D1 D0
    // 0  0  1  1  0  1  1  0
    // 即就是 36 H
    // 设置 8253/8254 芯片工作在模式 3 下
    outb(0x43, 0x36);

    // 拆分低字节和高字节
    u8 low = (u8)(divisor & 0xFF);
    u8 hign = (u8)((divisor >> 8) & 0xFF);
    
    // 分别写入低字节和高字节
    outb(0x40, low);
    outb(0x40, hign);
}

void init_idt()
{
    const SR_80 idtr =
    {
        (u16)sizeof(IDT_ENTRY) * IDT_MAX_ENTRIES - 1,
        (u64)&idt_entries[0]
    };

    idt_load(&idtr);

    pic_remap();

    u8 i = 0;
    for(; i < 32; i++)
    {
        idt_set(i, (u64)&idt_handler_array[i * IDT_HANDLER_SIZE],
            IDT_DESCRIPTOR_FAULT, 0);
    }
    
    for(;i < 48; i++)
    {
        idt_set(i, (u64)&idt_handler_array[i * IDT_HANDLER_SIZE],
            IDT_DESCRIPTOR_EXTERNAL, 0);
    }
}