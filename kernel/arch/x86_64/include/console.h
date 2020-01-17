#pragma once
#include <types.h>


enum Color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};


void console_initialize();
void console_setcolor(_u8 color);
_u8  console_getcolor();
void console_putentryat(char c, _u8 color, _u16 x, _u16 y);
void console_putchar(char c);
void console_write(const char *data, _u16 size);
void console_writez(const char *data);

static inline _u8 vga_entry_color(enum Color fg, enum Color bg) {
	return fg | bg << 4;
}

static inline _u16 vga_entry(unsigned char uc, _u8 color) {
	return (_u16) uc | (_u16) color << 8;
}
