//
// Created by wmj on 3/19/20.
//

#include <stdlib.h>
#include <stddef.h>

void *memcpy(void *dst, const void *src, size_t size) {
    void *t = dst;
    while (size--) {
        *dst = *src;
        dst++;
        src++;
    }
    return t;
}

void *memmove(void *dst, const void *src, size_t size) {
    return memcpy(dst, src, size);
}

void *memset(void *ptr, char value, size_t num) {
    void *t = ptr;
    while(num--) {
        *ptr = value;
        ptr++;
    }
    return t;
}
