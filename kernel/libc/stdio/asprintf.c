#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int asprintf(char **dst, const char *fmt, ...) {
    char *buf = malloc(2048);
    va_list va;
    va_start(va, fmt);
    int t = (int)vsnprintf(buf, 2048, fmt, va);
    va_end(va);
    *dst = buf;
    return t;
}