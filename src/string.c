#include <type.h>

void memset(void* dest, isize value, usize size)
{
    for(usize i = 0; i < size; i += sizeof(i8))
    {
        ((i8*)dest)[i] = value;
    }
}

void memcpy(void* dest, void* src, usize size)
{
    for(usize i = 0; i < size; i += sizeof(i8))
    {
        ((i8*)dest)[i] = ((i8*)src)[i];
    }
}