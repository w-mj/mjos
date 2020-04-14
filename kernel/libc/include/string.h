#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

size_t strlen(const char *);
int    strncmp(const char *s1, const char *s2, size_t n);
char *strchr(const char *s, int c);
char *strstr(const char *haystack, const char *needle);
char *strcpy(char *dst, const char *src);
char *strdup(const char *);
int strcmp(const char *s1, const char *s2);

void memcpy(void *dst, const void *src, _u16 size);
void memmove(void *dst, const void *src, _u16 size);
void memset(void *ptr, char value, _u16 num);

int memcmp(const void *a1, const void *a2, size_t n);


#ifdef __cplusplus
}
#endif
