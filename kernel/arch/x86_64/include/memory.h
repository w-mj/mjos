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
} FrameEntry;

void init_page(void *mmp_addr, u64 mmap_length);
void rebuild_kernel_page();

