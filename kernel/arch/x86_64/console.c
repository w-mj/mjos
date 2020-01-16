#include <console.h>
#include <types.h>

static inline _u8 vga_entry_color(enum Color fg, enum Color bg) {
	return fg | bg << 4;
}

static inline _u16 vga_entry(unsigned char uc, _u8 color) {
	return (_u16) uc | (_u16) color << 8;
}

static const _u16 VGA_WIDTH = 80;
static const _u16 VGA_HEIGHT = 25;

_u16 console_row;
_u16 console_column;
_u8 console_color;
_u16 *console_buffer;

void console_initialize(void) {
	console_row = 0;
	console_column = 0;
	console_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	console_buffer = (_u16 *) 0xB8000;
	_u16 ch = vga_entry(' ', console_color);
	for (_u16 y = 0; y < VGA_HEIGHT; y++) {
		for (_u16 x = 0; x < VGA_WIDTH; x++) {
			const _u16 index = y * VGA_WIDTH + x;
			console_buffer[index] = ch;
		}
	}
}

void console_setcolor(_u8 color) {
	console_color = color;
}

void console_putentryat(char c, _u8 color, _u16 x, _u16 y) {
	const _u16 index = y * VGA_WIDTH + x;
	console_buffer[index] = vga_entry(c, color);
}

void console_putchar(char c) {
	console_putentryat(c, console_color, console_column, console_row);
	// TODO: scroll up
	if (++console_column == VGA_WIDTH) {
		console_column = 0;
		if (++console_row == VGA_HEIGHT) {
			console_row = 0;
		}
	}
}

void console_write(const char *data, _u16 size) {
	for (_u16 i = 0; i < size; i++) {
		console_putchar(data[i]);
	}
}

void console_writez(const char *data) {
	while (*data != '\0') {
		console_putchar(*data);
		data++;
	}
}

