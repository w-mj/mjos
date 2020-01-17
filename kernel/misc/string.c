#include <string.h>


size_t strlen(const char *s) {
	const char *a = s;
	while (*a != '\0')
		a++;
	return a - s;
}
