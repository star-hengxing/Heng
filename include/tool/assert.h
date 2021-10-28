#pragma once

#include <printk.h>

#define assert(expr)                                                                               \
    if(expr)                                                                                       \
    {                                                                                              \
        /* pass */                                                                                 \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
        printk("[%s;%d] %s\n", __FILE__, __LINE__, #expr);                                         \
    }
