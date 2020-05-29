
#include <stdlib.h>
#include <string.h>


#ifndef TNAME
#ifdef TEST
#define TNAME(x) test_##x
#else
#define TNAME(x) x
#endif
#endif

char *TNAME(strdup)(const char *str) {
    size_t size = strlen(str);
    char *ans = (char*)malloc(size + 1);
    memcpy(ans, str, size + 1);
    return ans;
}