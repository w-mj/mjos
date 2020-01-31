#include <memory.h>
#include <delog.h>
#include <list.h> 
#include <base.h>
#include <list.h>
#include "early_kmalloc.h"

FrameEntry *mem_pool = NULL;  // 存放所有页框的数组
int mem_frame_cnt = 0;
static void init_mem_area(u64 addr, u64 len) {
	u64 addr_end = addr + len;
	// 内存头部和尾部少于一个页的空间都舍弃 
	addr_end = ROUND_DOWN(addr_end, PAGESIZE); 
	addr     = ROUND_UP(addr, PAGESIZE);
	// 低于16M的空间为DMA区域，不放在内存池中
	if (addr < (16 * (1 << 20))) {
		addr = (16 * (1 << 20));
	}
	len = addr_end - addr;
	int frame_cnt = len / PAGESIZE;  // 页框数
	logi("mem area addr: 0x%x%08x, len: %d, frames: %d", h32(addr), l32(addr), len, frame_cnt);
	if (mem_pool == NULL)
		mem_pool = (FrameEntry*)early_kmalloc(frame_cnt * sizeof(FrameEntry));
	else 
		// early_kmalloc 只在这里被连续调用，因此多次分配的空间一定是连续的
		early_kmalloc(frame_cnt * sizeof(FrameEntry));
	FrameEntry *mmap = mem_pool + frame_cnt;
	mem_frame_cnt += frame_cnt;
	while (frame_cnt--) {
		mmap->address = (void*)addr;
		addr += PAGESIZE;
		mmap->count = 0;
		mmap->state = AVAILABLE;
		mmap++;
	}
}

void init_page(void *mmap_addr, u64 mmap_length) {
	logi("init page");
	early_kmalloc_init();

	multiboot_memory_map_t *mmap;
	// 遍历内存表
	for (mmap = (multiboot_memory_map_t *)mmap_addr;
			(u64) mmap < (u64)mmap_addr + mmap_length;
			mmap = (multiboot_memory_map_t *)((unsigned long ) mmap + mmap->size + sizeof(mmap->size))) {
		// 跳过不可用内存区域
		if (mmap->type != 1)
			continue;
		// 跳过低于1M的内存区域
		if (mmap->addr < (1 << 20))
			continue;
		init_mem_area(mmap->addr, mmap->len);
	}
	logi("frames count: %d", mem_frame_cnt);
}

void rebuild_kernel_page() {
	
}
