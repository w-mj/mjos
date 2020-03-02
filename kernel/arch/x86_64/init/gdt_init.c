#include <types.h>
#include <string.h>
#include <asm.h>
#include <base.h>
#include <delog.h>
#include <cpu.h>
#include <early_kmalloc.h>

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

u64 *gdt = NULL;

struct Gdtr{
	u16  size;
	u64  address;
}__PACKED;
void load_gdtr(void *);

void gdt_init() {
	if (cpu_activated == 0) {
		logi("gdt init");
		gdt = early_kmalloc((6 + cpu_count() * 2) * sizeof(u64));
		gdt[0] = 0;   // dummy
		gdt[1] = 0x00a0980000000000UL;  // kernel code
		gdt[2] = 0x00c0920000000000UL;  // kernel data
		gdt[3] = 0x00a0f80000000000UL;  // 占位，使经常被访问到的段处于同一个32B缓存中
		gdt[4] = 0x00a0f80000000000UL;  // user code
		gdt[5] = 0x00c0f20000000000UL;  // user data
	}
	struct Gdtr gdtr;
	gdtr.size = (6 + cpu_count() * 2) * sizeof(u64) - 1;
	gdtr.address = (u64)gdt;
	// ASM("lgdt %0"::"m" (gdtr));
	load_gdtr(&gdtr);
}
