#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"

void *malloc(size_t size);
void *realloc(void *addr, size_t size);
void *calloc(size_t num, size_t size);

void free(void *addr);

void abort();

static inline int time(int a) {return 0;}


long strtol (const char *__restrict s, char **__restrict ptr, int base);

#ifdef __cplusplus
}
#endif
