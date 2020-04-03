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

#include "time.h"

long strtol (const char *__restrict s, char **__restrict ptr, int base);

#ifdef __cplusplus
}
#endif
