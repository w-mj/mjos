#pragma once
#include <types.h>
#include <memory.h>
#include <attribute.h>

#include <multiboot.h>


extern u64 KERNEL_VMA, KERNEL_LMA;
#define ABSOLUTE(x) (((u64)(x) - ((u64)&KERNEL_VMA)) + ((u64)&KERNEL_LMA))
#define VIRTUAL(x)  (((u64)(x) + (((u64)&KERNEL_VMA)) - ((u64)&KERNEL_LMA)))


