#pragma once
#ifdef __cplusplus
extern "C" {
#endif


#include <stdarg.h>
#include <types.h>

size_t snprintf (char *, size_t, const char *, ...);
size_t vsnprintf (char *, size_t, const char *, va_list);
int asprintf (char **, const char *, ...);

#ifdef __cplusplus
}
#endif
