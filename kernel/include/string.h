#pragma once
#include <types.h>

size_t strlen(const char *);
int    strncmp(const char *s1, const char *s2, size_t n);

void memcpy(void *dst, void *src, _u16 size);
void memset(void *ptr, char value, _u16 num);
