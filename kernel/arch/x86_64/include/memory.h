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
	u64 address;  // 起始地址（页框号）
	enum PageStatus state;  // 状态
	ListEntry next;  // 下一个
} FrameEntry;

void init_page(void *mmp_addr, u64 mmap_length);
FrameEntry *frame_alloc();
void frame_release(FrameEntry *frame);
typedef u64 PageTable;

#define PMLOFFSET(x, n) (((x) >> ((n - 1) * 9 + 12)) & 0x1ff)
#define PTENTRYADDR(x) ((x) & (~0xfff))
#define PRESENT(x)     ((x) & 0x01)


