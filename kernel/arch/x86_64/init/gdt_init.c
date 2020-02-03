#include <attribute.h>
#include <types.h>
#include <string.h>
#include <asm.h>
#include <base.h>
#include <delog.h>

typedef struct {
	u16 limit;
	u16 base_low;
	u8  base_mid;
	u8  type : 4,
		s    : 1,
		dpl  : 2,
		p    : 1;
	u8  limit_high : 4,
		a          : 1,
		pad        : 1,
		db         : 1,
		g          : 1;
	u8  base_high;
} __PACKED GD;

#define GDT_LENGTH 8
u64 gdt[GDT_LENGTH];

static struct {
	u16  size;
	u64  address;
}__PACKED gdtr;
void load_gdtr(void *);
void gdt_init() {
	logi("gdt init");
	gdt[0] = 0;   // dummy
	gdt[1] = 0x00a0980000000000UL;  // kernel code
	gdt[2] = 0x00c0920000000000UL;  // kernel data
	gdt[3] = 0;    // 占位，使经常被访问到的段处于同一个32B缓存中
	gdt[4] = 0x00c0f20000000000UL;  // user data
	gdt[5] = 0x00a0f80000000000UL;  // user code
	gdtr.size = GDT_LENGTH * 8 - 1;
	gdtr.address = (u64)gdt;
	// ASM("lgdt %0"::"m" (gdtr));
	load_gdtr(&gdtr);
}
