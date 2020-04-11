#pragma once
#ifdef __cplusplus
extern "C" {
#endif


#include <stdarg.h>
#include <types.h>

struct __FILE {
    int fno;
    int size;
};

typedef struct __FILE FILE;

//extern struct FILE *stderr;
//extern struct FILE *stdout;
//extern struct FILE *stdin;

int snprintf (char *, size_t, const char *, ...);
int vsnprintf (char *, size_t, const char *, va_list);
int asprintf (char **, const char *, ...);

int fprintf(FILE *f, const char * fmt, ...);
#define printf(...) fprintf(stdout, __VA_ARGS__)

void fprintf_init();


#ifdef __cplusplus
}
#endif
