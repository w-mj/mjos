#pragma once
#include <types.h>
#include <boot.h>

enum PageStatus {
	AVAILABLE,
	OCCUPIED
};

typedef struct {
	int count;
	void *address;
	PageStatus state;
} Page;

typedef struct _PagePool {
	Page* page_pool;
	struct _PagePool *next;
} PagePools;

void init_page(void *mmp_addr, u64 mmap_length);
void rebuild_kernel_page();
