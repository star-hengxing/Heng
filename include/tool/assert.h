#pragma once

#include <printk.h>

#define assert(expr) printk("[%s;%d] %s\n", __FILE__, __LINE__, #expr)