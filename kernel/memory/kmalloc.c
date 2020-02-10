#include <memory.h>
#include <delog.h>
#include <list.h>
#include <base.h>
#include <types.h>
#include <string.h>

typedef struct __MemBlock {
	u64 addr;
	size_t size;
	ListEntry next;
} MemBlock;

extern u64 heap_end;
extern u64 kernel_pml4;

static ListEntry mem_list;

void kmalloc_init() {
	list_init(&mem_list);
}

// 分配内存，使用最优匹配法
void *kmalloc(size_t size) {
	if (size == 0)
		return NULL;
	ListEntry *c;
	MemBlock *min_gap_mem = NULL;
	u32 min_gap = -1;   // 内存块和目标尺寸之间的最小差距

	foreach(c, mem_list) {
		MemBlock *mb = list_entry(c, MemBlock, next);
		if (mb->size < size + sizeof(size_t))  // 多出4字节保存内存块长度
			continue;
		if (min_gap > mb->size - size) {
			min_gap_mem = mb;
			min_gap = mb->size - size;
		}
	}

	if (min_gap_mem == NULL) {
		// 需要新分配pages个页
		int pages = ROUND_UP(size, PAGESIZE) / PAGESIZE;
		while (pages--) {
			void *ptr = kernel_page_alloc();

		}
	}

	min_gap_mem->size -= size + sizeof(size_t);
	*(size_t*)min_gap_mem->addr = size;
	void *to_ret = (void*)(min_gap_mem->addr + sizeof(size_t));
	min_gap_mem->addr += size + sizeof(size_t);
	return to_ret;
}
