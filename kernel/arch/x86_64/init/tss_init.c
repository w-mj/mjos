#include <types.h>
#include <string.h>
#include <asm.h>
#include <delog.h>
#include <base.h>
#include <cpu.h>

typedef struct tss {
    u32 reserved1;
    u32 rsp0_lower;
    u32 rsp0_upper;
    u32 rsp1_lower;
    u32 rsp1_upper;
    u32 rsp2_lower;
    u32 rsp2_upper;
    u32 reserved2;
    u32 reserved3;
    u32 ist1_lower;
    u32 ist1_upper;
    u32 ist2_lower;
    u32 ist2_upper;
    u32 ist3_lower;
    u32 ist3_upper;
    u32 ist4_lower;
    u32 ist4_upper;
    u32 ist5_lower;
    u32 ist5_upper;
    u32 ist6_lower;
    u32 ist6_upper;
    u32 ist7_lower;
    u32 ist7_upper;
    u32 reserved4;
    u32 reserved5;
    u16 reserved6;
    u16 io_map_base;
} __PACKED Tss;

__PERCPU Tss tss;
extern u64 *gdt;
extern u64 boot_stack_top;
void tss_init() {
	if (cpu_activated == 0) {
		logi("tss init");
	}
    u64 tss_size = (u64)sizeof(Tss);
    u64 tss_addr = (u64)thiscpu_ptr(tss);
	Tss *tssl     = (Tss *)tss_addr;
    memset((void *) tss_addr, 0, tss_size);
	tssl->rsp0_lower = ((u64)&boot_stack_top) & 0xffffffff;
	tssl->rsp0_upper = ((u64)&boot_stack_top) >> 32;


    u64 lower = 0UL;
    u64 upper = 0UL;
    lower |=  tss_size        & 0x000000000000ffffUL;   // limit [15:0]
    lower |= (tss_addr << 16) & 0x000000ffffff0000UL;   // base  [23:0]
    lower |= (tss_size << 32) & 0x000f000000000000UL;   // limit [19:16]
    lower |= (tss_addr << 32) & 0xff00000000000000UL;   // base  [31:24]
    lower |=                    0x0000e90000000000UL;   // present 64bit ring3
    upper  = (tss_addr >> 32) & 0x00000000ffffffffUL;   // base  [63:32]

    gdt[2 * cpu_index() + 6] = lower;
    gdt[2 * cpu_index() + 7] = upper;

	// _si(cpu_index());
	// die();
	u16 tr = ((2 * cpu_index() + 6) << 3) | 3;
	_sx(tr);
	ASM("ltr  %0"::"r" (tr));
	_pos();
	// logi("tss init finish");
}

void set_rsp0(u64 rsp0) {
    thiscpu_var(tss).rsp0_lower = rsp0 & 0xffffffff;
    thiscpu_var(tss).rsp0_upper = rsp0 >> 32;
}
