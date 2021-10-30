#pragma once

#define INTERRUPT_ENABLE() asm volatile("sti")

#define INTERRUPT_DISBALE() asm volatile("cli")