#pragma once

#include <interrupt/idt.h>
#include <type.h>

typedef enum
{     
	TASK_RUNNING,
	TASK_READY,
	TASK_BLOCKED,
	TASK_WAITING,
	TASK_HANGING,
	TASK_DIED
} task_status;

typedef struct
{
	interrupt_regs context;
	task_status task_status;
	// u64 tid;
	u64 stack_top;
    u64 next;
} TCB;

// void create_thread(void* fn);

void init_kernel_thread();