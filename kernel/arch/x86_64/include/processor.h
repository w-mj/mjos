#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "base.h"
#include "boot.h"

// 与CPU相关的结构
#define IO_BITMAP_BITS	65536
#define IO_BITMAP_BYTES	(IO_BITMAP_BITS/8)
struct __Tss {
	u32 reserved1;
	u64 sp0;
	u64 sp1;
	u64 sp2;
	u64 reserved2;
	u64 ist[7];
	u32 reserved3;
	u32 reserved4;
	u16 reserved5;
	u16 io_bitmap_base;
	u8  io_bitmap[IO_BITMAP_BYTES+ 1];
} __PACKED __ALIGNED(PAGESIZE);
typedef struct __Tss Tss;


struct __Regs {
	u64 rax;
	u64 rbx;
	u64 rcx;
	u64 rdx;
	u64 rsi;
	u64 rdi;
	u64 rsp;
	u64 rbp;
	u64 r8;
	u64 r9;
	u64 r10;
	u64 r11;
	u64 r12;
	u64 r13;
	u64 r14;
	u64 r15;
} __PACKED;
typedef struct __Regs Regs;


struct __ThreadInfo {
	Regs regs;
};

#ifdef __cplusplus
}
#endif
