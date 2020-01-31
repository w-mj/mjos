#include <types.h>
#include <boot.h>
#include <base.h>
#include <delog.h>

/*
 * 在内核页表构建完成之前提供malloc服务
 * early_kmelloc分配的内存只用于存放页表基础数据结构
 * 分配的内存不再回收
 */
extern u64 _bss_end;
u64 heap_end;
u64 heap_ptr;

void early_kmalloc_init(void) {
	heap_ptr = heap_end = (u64)&_bss_end;
	heap_end = ROUND_UP(heap_end, PAGESIZE);
	logi("init early kmelloc heap_ptr: %x heap_end: %x", heap_end, heap_ptr);
}

void *early_kmalloc(size_t size) {
	void *ptr = (void*)heap_ptr;
	heap_ptr += size;
	while (heap_ptr >= heap_end)
		heap_end += PAGESIZE;
	// logi("early kmelloc %d, heap_ptr: %x heap_end: %x", size, heap_ptr, heap_end);
	return ptr;
}
