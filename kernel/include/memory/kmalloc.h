#pragma once
#include <types.h>

void kmalloc_init();
void *kmalloc_s(size_t size);
void kfree_s(size_t size, void *addr);
