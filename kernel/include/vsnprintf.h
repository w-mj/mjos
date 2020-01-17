#pragma once

#include <stdarg.h>
#include <types.h>

size_t snprintf (char *, size_t, const char *, ...);
size_t vsnprintf (char *, size_t, const char *, va_list);

