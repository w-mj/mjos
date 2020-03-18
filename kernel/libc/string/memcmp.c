#include <stddef.h>

int memcmp(const void *a1, const void *a2, size_t n) {
    const char *s1 = (const char *)a1;
    const char *s2 = (const char *)a2;
    while (n--) {
        if (*s1 != *s2)
            return *s1 - *s2;
        s1++;
        s2++;
    }
    return 0;
}