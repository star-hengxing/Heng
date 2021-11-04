#include <kernel/thread.h>
#include <kernel/printk.h>

#include <lib/string.h>

#include <interrupt/idt.h>

#include <memory/pm.h>

#include <x86/asm.h>
#include <x86/x86.h>
#include <x86/vga.h>
#include <x86/pt.h>


#define MAX_THREAD 4

typedef struct
{
    TCB thread[MAX_THREAD];
    u64 current;
} task_queue;

static task_queue* queue;

#define INIT_CONTEXT_REGS(tcb, RIP, CS, EFLAG, SS) \
    do                                             \
    {                                              \
        tcb->context.regs.rip    = (u64)RIP;       \
        tcb->context.regs.cs     = CS;             \
        tcb->context.regs.eflags = EFLAG;          \
        tcb->context.regs.rsp    = tcb->stack_top; \
        tcb->context.regs.ss     = SS;             \
    } while(0)

// void create_thread(usize index, void* fn)
// {
//     TCB* temp = &queue->thread[index];
//     temp->task_status = TASK_READY;
//     temp->stack_top = 0;
//     CONTEXT_REGS_INIT(temp, fn, 0x8, 0x200, 0x10);
// }

void thread_1()
{
    while(1)
    {
        INTERRUPT_DISBALE();
        printk("Thread_1 ");
        INTERRUPT_ENABLE();
    }
}
void thread_2()
{
    while(1)
    {
        INTERRUPT_DISBALE();
        printk("Thread_2 ");
        INTERRUPT_ENABLE();
    }
}
void thread_3()
{
    while(1)
    {
        INTERRUPT_DISBALE();
        printk("Thread_3 ");
        INTERRUPT_ENABLE();
    }
}

void scheduling(interrupt_regs* c)
{
    memcpy(&queue->thread[queue->current].context, c, sizeof(interrupt_regs));
    queue->current = queue->thread[queue->current].next;
    memcpy(c, &queue->thread[queue->current].context, sizeof(interrupt_regs));
}

void init_kernel_thread()
{
    // 2M page
    const p_page* page = p_page_alloc();
    
    const u64 virtual_base = kernel_virtual_memory_map(1, page->base);

    memset((void*)virtual_base, 0, PAGE_TABLE_MAX_ENTRY * PAGE_SIZE);

    queue = (task_queue*)virtual_base;
    queue->current = 0;

    queue->thread[0].stack_top = virtual_base + PAGE_SIZE * 2;
    queue->thread[1].stack_top = virtual_base + PAGE_SIZE * 3;
    queue->thread[2].stack_top = virtual_base + PAGE_SIZE * 4;
    queue->thread[3].stack_top = virtual_base + PAGE_SIZE * 5;

    queue->thread[0].next = 1;
    queue->thread[1].next = 2;
    queue->thread[2].next = 3;
    queue->thread[3].next = 0;

    INIT_CONTEXT_REGS((&queue->thread[0]), 0,        0x8, 0x200, 0x10);
    INIT_CONTEXT_REGS((&queue->thread[1]), thread_1, 0x8, 0x200, 0x10);
    INIT_CONTEXT_REGS((&queue->thread[2]), thread_2, 0x8, 0x200, 0x10);
    INIT_CONTEXT_REGS((&queue->thread[3]), thread_3, 0x8, 0x200, 0x10);

    interrupt_handler_register(32, scheduling);
}