#pragma once
#include <types.h>
#include <multiboot.h>

#define PAGEOFFSET 12
#define PAGESIZE (1 << PAGEOFFSET)
extern u64 KERNEL_VMA, KERNEL_LMA;
#define ABSOLUTE(x) (((u64)(x) - ((u64)&KERNEL_VMA)) + ((u64)&KERNEL_LMA))
#define VIRTUAL(x)  (((u64)(x) + (((u64)&KERNEL_VMA)) - ((u64)&KERNEL_LMA)))



