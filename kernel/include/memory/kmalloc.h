#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

void kmalloc_init();
void *kmalloc_s(size_t size);
void kfree_s(size_t size, void *addr);

#ifdef __cplusplus
}
#endif
