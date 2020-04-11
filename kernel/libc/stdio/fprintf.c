#include <stdio.h>

FILE *stdout;
FILE *stdin;
FILE *stderr;
static FILE _stdout;
static FILE _stdin;
static FILE _stderr;

#include <stdarg.h>
#include <console.h>
#include <serial.h>

#define BUF_SIZE 4096
static char fprintf_buf[BUF_SIZE];
int fprintf(FILE *f, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    int n = vsnprintf(fprintf_buf, BUF_SIZE, fmt, va);
    va_end(va);
    fprintf_buf[n] = 0;

    u8 color = console_getcolor();
    if (f->fno == 0) {
        console_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    } else if (f->fno == 2) {
        console_setcolor(vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK));
    }
    char *s = fprintf_buf;
    console_writez(s);
    while (*s != 0) {
        serial_putchar(*s);
        s++;
    }
    console_setcolor(color);
    return n;
}

#include <stdlib.h>
#include <string.h>
int asprintf(char **dst, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    int t = (int)vsnprintf(fprintf_buf, BUF_SIZE, fmt, va);
    va_end(va);
    char *buf = malloc(t);
    memcpy(buf, fprintf_buf, t);
    *dst = buf;
    buf[t] = 0;
    return t;
}


void fprintf_init() {
    stdout = &_stdout;
    stdin  = &_stdin;
    stderr = &_stderr;
}