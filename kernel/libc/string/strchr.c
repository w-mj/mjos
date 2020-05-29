#include <stddef.h>

#ifndef TNAME
#ifdef TEST
#define TNAME(x) test_##x
#else
#define TNAME(x) x
#endif
#endif

char *TNAME(strchr)(const char *s, int i) {
    while (*s != '\0') {
        if (*s == i)
            return (char *)s;
        s++;
    }
    return NULL;
}
