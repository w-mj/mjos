#pragma once
#include <types.h>
#include <boot.h>
#include <list.h>

enum PageStatus {
	AVAILABLE,
	OCCUPIED
};

typedef struct {
	int count;  // 引用计数
	void *address;  // 起始地址
	enum PageStatus state;  // 状态
	ListEntry next;  // 下一个
} FrameEntry;

void init_page(void *mmp_addr, u64 mmap_length);

typedef u64 PageTable;

#define PML4OFFSET(x) (((x) >> 39) & 0x7f)
#define PML3OFFSET(x) (((x) >> 30) & 0x7f)
#define PML2OFFSET(x) (((x) >> 21) & 0x7f)
#define PML1OFFSET(x) (((x) >> 12) & 0x7f)
#define PMLOFFSET(x, n) (((x) >> ((n - 1) * 9 + 12)) & 0x7f)
#define PTENTRYADDR(x) (x & (~0xfff))


