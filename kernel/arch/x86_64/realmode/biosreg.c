#include <copy.h>
#include <asm.h>
#include <biosreg.h>
#include <processor-flags.h>

void initregs(struct biosregs *reg)
{
	memset(reg, 0, sizeof *reg);
	reg->eflags |= X86_EFLAGS_CF;
	reg->ds = ds();
	reg->es = ds();
	reg->fs = fs();
	reg->gs = gs();
}

