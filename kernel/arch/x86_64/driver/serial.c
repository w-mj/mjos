#include <serial.h>
#include <types.h>
#include <asm.h>
#include <string.h>
#include <delog.h>

#define DEFAULT_SERIAL_PORT 0x3f8 /* ttyS0 */
u16 early_serial_base;

#define DLAB		0x80

#define TXR             0       /*  Transmit register (WRITE) */
#define RXR             0       /*  Receive register  (READ)  */
#define IER             1       /*  Interrupt Enable          */
#define IIR             2       /*  Interrupt ID              */
#define FCR             2       /*  FIFO control              */
#define LCR             3       /*  Line control              */
#define MCR             4       /*  Modem control             */
#define LSR             5       /*  Line Status               */
#define MSR             6       /*  Modem Status              */
#define DLL             0       /*  Divisor Latch Low         */
#define DLH             1       /*  Divisor latch High        */


#define DEFAULT_BAUD 9600

static void early_serial_init(int port, int baud) {
	unsigned char c;
	unsigned divisor;
	outb(0x3, port + LCR);	/* 8n1 */
	outb(0, port + IER);	/* no interrupt */
	outb(0, port + FCR);	/* no fifo */
	outb(0x3, port + MCR);	/* DTR + RTS */
	divisor	= 115200 / baud;
	c = inb(port + LCR);
	outb(c | DLAB, port + LCR);
	outb(divisor & 0xff, port + DLL);
	outb((divisor >> 8) & 0xff, port + DLH);
	outb(c & ~DLAB, port + LCR);
	early_serial_base = port;
}


#define BASE_BAUD (1843200/16)
static unsigned int probe_baud(int port) {
	unsigned char lcr, dll, dlh;
	unsigned int quot;
	lcr = inb(port + LCR);
	outb(lcr | DLAB, port + LCR);
	dll = inb(port + DLL);
	dlh = inb(port + DLH);
	outb(lcr, port + LCR);
	quot = (dlh << 8) | dll;
	return BASE_BAUD / quot;
}

void serial_initialize(void) {
	early_serial_init(DEFAULT_SERIAL_PORT, DEFAULT_BAUD);
	logi("initialize serial.");
}

int is_transmit_empty() {
	return inb(early_serial_base + LSR) & 0x20;
}

void serial_putchar(char a) {
	// while (is_transmit_empty() == 0);
	outb(a, early_serial_base + TXR);
}

int serial_received() {
	return inb(early_serial_base + LSR) & 1;
}

char serial_getchar() {
	// while (serial_received() == 0);
	return inb(early_serial_base);
}

