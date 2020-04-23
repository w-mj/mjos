#include <stddef.h>


char *strchr(const char *s, int i) {
    while (*s != '\0') {
        if (*s == i)
            return (char *)s;
        s++;
    }
    return NULL;
}
