#include <memory.h>
#include <delog.h>
#include <list.h> 
#include <base.h>
#include <list.h>
#include "early_kmalloc.h"

// Buddy 算法，11个连续页框列表
ListEntry mem_pool[11];
int mem_pool_frames[11] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

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
	int i = 10;
	for (; i >= 0; i--) {
		while (frame_cnt >= mem_pool_frames[i]) {
			frame_cnt -= mem_pool_frames[i];
			FrameEntry *new_entry = (FrameEntry*)early_kmalloc(sizeof(FrameEntry));
			new_entry->count = 0;
			new_entry->address = (void*)addr;
			addr += mem_pool_frames[i] * PAGESIZE;
			list_add(&new_entry->list_head, &mem_pool[i]);
		}
	}
}

void init_page(void *mmap_addr, u64 mmap_length) {
	logi("init page");
	early_kmalloc_init();
	repet(11) {
		list_init(&mem_pool[_]);
	}

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
	int sum = 0;
	repet(11) {
		ListEntry *head = &mem_pool[_];
		ListEntry *cur = head->next;
		int i = 0;
		while(cur != head) {
			FrameEntry *frame = list_entry(cur, FrameEntry, list_head);
			// logi("Frame addr %d", frame->address);
			cur = cur->next;
			i++;
		}
		logi("Frame buddy level %d, count %d", 1 << _, i);
		sum += i * mem_pool_frames[_];
	}
	logi("sum frames %d", sum);
}

void rebuild_kernel_page() {
	
}
