//
// Created by wmj on 3/19/20.
//

#include <stdlib.h>
#include <stddef.h>

void *memcpy(void *dst, const void *src, size_t size) {
    char *s1 = (char *)dst;
    char *s2 = (char *)src;
    while (size--) {
        *s1= *s2;
        s1++;
        s2++;
    }
    return dst;
}

void *memmove(void *dst, const void *src, size_t size) {
    return memcpy(dst, src, size);
}

void *memset(void *ptr, char value, size_t num) {
    char *t = (char *)ptr;
    while(num--) {
        *t= value;
        t++;
    }
    return ptr;
}
