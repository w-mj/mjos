#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

void  early_kmalloc_init(u64);
void *early_kmalloc(size_t size);
void early_kmalloc_depercated();

#ifdef __cplusplus
}
#endif
