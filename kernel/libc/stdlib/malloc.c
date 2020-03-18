#include <memory/kmalloc.h>

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

void *malloc(size_t size) {
    return kmalloc(size);
}

void *calloc(size_t num, size_t size) {
    return malloc(num * size);
}


void *realloc(void *addr, size_t size) {
    void *naddr = malloc(size);
    memcpy(naddr, addr, size);
    free(addr);
    return naddr;
}

void free(void *addr) {
    kfree(addr);
}

void abort() {
    while (1);
}
