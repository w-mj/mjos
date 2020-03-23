
#include <stdlib.h>
#include <string.h>

char *strdup(const char *str) {
    size_t size = strlen(str);
    char *ans = malloc(size + 1);
    memcpy(ans, str, size + 1);
    return ans;
}