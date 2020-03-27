//
// Created by wmj on 3/19/20.
//

#include <iostream.hpp>
#include <console.h>
#include <serial.h>
#include <stdio.h>

using namespace os;

ostream os::cout(0);
ostream os::cerr(2);

ostream::ostream(int fno) {
    this->fno = fno;
}

ostream &ostream::operator<<(const char * s) {
    u8 color = console_getcolor();
    if (fno == 0) {
        console_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    } else if (fno == 2) {
        console_setcolor(vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK));
    }
    console_writez(s);
    while (*s != 0) {
        serial_putchar(*s);
        s++;
    }
    console_setcolor(color);
    return *this;
}

ostream &ostream::operator<<(const string & str) {
    return (*this)<<str.c_str();
}

ostream &ostream::operator<<(int a) {
    char buf[128];
    snprintf(buf, 128, "%d", a);
    return (*this) << buf;
}
