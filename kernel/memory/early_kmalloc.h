#pragma once
#include <types.h>

void  early_kmalloc_init(void);
void *early_kmalloc(size_t size);
void early_kmalloc_depercated();
extern u64 KERNEL_LMA, KERNEL_VMA;
