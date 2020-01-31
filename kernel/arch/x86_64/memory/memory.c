#include <memory.h>
#include <delog.h>
 

void init_page(void *mmap_addr, u64 mmap_length) {
	logi("init page");
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
	}
}

void rebuild_kernel_page() {
	
}
