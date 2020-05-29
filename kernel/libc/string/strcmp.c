
#ifndef TNAME
#ifdef TEST
#define TNAME(x) test_##x
#else
#define TNAME(x) x
#endif
#endif

int TNAME(strcmp)(const char *s1, const char *s2) {
    while (*s1 != 0 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (int)*s1 - (int)*s2;
}