#include <biosreg.h>
void init16(void) {
	struct biosregs regs;
	initregs(&regs);
	regs.cx = 0x2000;
	regs.ah = 0x01;
	intcall(0x10, &regs, &regs);
}
