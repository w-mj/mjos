#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

size_t strlen(const char *);
int    strncmp(const char *s1, const char *s2, size_t n);

void memcpy(void *dst, const void *src, _u16 size);
void memset(void *ptr, char value, _u16 num);

#include "vsnprintf.h"


#ifdef __cplusplus
}
#endif
