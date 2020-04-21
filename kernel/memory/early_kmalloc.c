#include <types.h>
#include <boot.h>
#include <base.h>
#include <delog.h>

/*
 * 在内核页表构建完成之前提供malloc服务
 * early_kmelloc分配的内存只用于存放页表基础数据结构
 * 分配的内存不再回收
 */
u64 heap_end;
u64 heap_ptr;
static u8 inited = 0, available=0;

void early_kmalloc_init(u64 kernel_end) {
	if (inited) {
		loge("early_kmalloc is already inited.");
		return;
	}
	heap_ptr = heap_end = kernel_end;
	heap_end = ROUND_UP(heap_end, PAGESIZE);
	logi("init early_kmalloc heap_ptr: %llx heap_end: %llx", heap_ptr, heap_end);
	inited = 1;
	available = 1;
}

void early_kmalloc_depercated() {
	logd("early_kmalloc depercated");
	available = 0;
}

void *early_kmalloc(size_t size) {
	if (!inited) {
		loge("early_kmalloc not inited");
		return NULL;
	}
	if (!available) {
		loge("early_kmalloc can not be used anymode.");
		return NULL;
	}
	void *ptr = (void*)heap_ptr;
	heap_ptr += size;
	while (heap_ptr >= heap_end)
		heap_end += PAGESIZE;
	logi("early_kmalloc %d, start: %llx heap_ptr: %llx heap_end: %llx",
			size, ptr, heap_ptr, heap_end);
	return ptr;
}
