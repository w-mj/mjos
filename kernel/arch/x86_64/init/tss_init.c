#include <types.h>
#include <attribute.h>
#include <string.h>
#include <asm.h>
#include <delog.h>

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

Tss tss;
extern u64 *gdt;

void tss_init() {
	logi("tss init");
    u64 tss_size = (u64) sizeof(Tss);
    u64 tss_addr = (u64)(&tss);
    memset((void *) tss_addr, 0, tss_size);

    u64 lower = 0UL;
    u64 upper = 0UL;
    lower |=  tss_size        & 0x000000000000ffffUL;   // limit [15:0]
    lower |= (tss_addr << 16) & 0x000000ffffff0000UL;   // base  [23:0]
    lower |= (tss_size << 32) & 0x000f000000000000UL;   // limit [19:16]
    lower |= (tss_addr << 32) & 0xff00000000000000UL;   // base  [31:24]
    lower |=                    0x0000e90000000000UL;   // present 64bit ring3
    upper  = (tss_addr >> 32) & 0x00000000ffffffffUL;   // base  [63:32]

    gdt[6] = lower;
    gdt[7] = upper;

	u64 tr = (6 << 3) | 3;
	ASM("ltr  %0"::"m" (tr));
}
