#include <console.h>
#include <types.h>
#include <copy.h>



static const _u16 VGA_WIDTH = 80;
static const _u16 VGA_HEIGHT = 25;

_u16 console_row;
_u16 console_column;
_u8 console_color;
_u16 *console_buffer;

void console_initialize(void) {
	console_row = 0;
	console_column = 0;
	console_buffer = (_u16 *) 0xB8000;
	console_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
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

_u8 console_getcolor() {
	return console_color;
}

void console_putentryat(char c, _u8 color, _u16 x, _u16 y) {
	const _u16 index = y * VGA_WIDTH + x;
	console_buffer[index] = vga_entry(c, color);
}

static void scroll() {
	const _u16 row_size = VGA_WIDTH * 2;
	for (_u8 y = 0; y < VGA_HEIGHT - 1; y++) {
		// 后一行复制到前一行
		memcpy(
		    console_buffer + y * VGA_WIDTH,
		    console_buffer + (y + 1) * VGA_WIDTH,
		    row_size);
	}
	console_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	_u16 ch = vga_entry(' ', console_color);
	_u16 x = 0;
	_u16 y = VGA_HEIGHT - 1;
	for (; x < VGA_WIDTH; x++) {
		console_buffer[y * VGA_WIDTH + x] = ch;
	}
}

void console_putchar(char c) {
	if (c == '\r') {
		console_column = 0;
	} else if (c == '\n') {
		console_row += 1;
	} else if (c < 32 || c > 126) {
		// 跳过不可显示字符
	} else {
		console_putentryat(c, console_color, console_column, console_row);
		console_column++;
	}
	if (console_column == VGA_WIDTH) {
		console_row++;
		console_column = 0;
	}
	if (console_row == VGA_HEIGHT) {
		scroll();
		console_column = 0;
		console_row = VGA_HEIGHT - 1;
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

