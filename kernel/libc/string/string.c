#include <string.h>


size_t strlen(const char *s) {
	const char *a = s;
	while (*a != '\0')
		a++;
	return a - s;
}

int strncmp(const char *s1, const char *s2, size_t n) {
	while (n > 0 && (*s1) == (*s2)) {
		s1++;
		s2++;
		n--;
	}
	if (n == 0)
		return 0;
	return *s1 - *s2;
}

char *strcpy(char *dst, const char *src) {
    char *t = dst;
    while (*src) {
        *dst = *src;
        dst++;
        src++;
    }
	*dst = 0;
    return t;
}
/*
void memcpy(char *dst, const char*src, _u16 size) {
    while (size--) {
        *dst = *src;
        dst++;
        src++;
    }
}
void memset(char *ptr, char value, _u16 num) {
    while (num--) {
        *ptr++ = value;
    }
}
*/