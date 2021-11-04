#include <lib/string.h>
#include <type.h>

void memset(void* dest, isize value, usize size)
{
    asm
    (
        // "mov %0, %rdi\n"
        "mov %0, %%rax\n"
        "mov %1, %%rcx\n"
        "rep stosb\n"
        :: "r"(value), "r"(size)
    );
}

void memcpy(void* dest, void* src, usize size)
{
    for(usize i = 0; i < size; i += sizeof(i8))
    {
        ((i8*)dest)[i] = ((i8*)src)[i];
    }
}