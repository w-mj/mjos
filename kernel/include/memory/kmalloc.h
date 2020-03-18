#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

void kmalloc_init();
void *kmalloc_s(size_t size);
void kfree_s(size_t size, void *addr);

void *kmalloc(size_t size);
void kfree(void *ptr);
#ifdef __cplusplus
}
#endif
