#include <types.h>
#include <boot.h>
#include <base.h>
#include <delog.h>

extern u64 _bss_end;
u64 heap_end;
u64 heap_ptr;

void init_early_kmelloc(void) {
	heap_ptr = heap_end = (u64)&_bss_end;
	heap_end = ROUND_UP(heap_end, PAGESIZE);
	logi("init early kmelloc heap_ptr: %x heap_end: %x", heap_end, heap_ptr);
}

void *early_kmelloc(size_t size) {
	void *ptr = (void*)heap_ptr;
	heap_ptr += size;
	while (heap_ptr >= heap_end)
		heap_end += PAGESIZE;
	logi("early kmelloc %d, heap_ptr: %x heap_end: %x", size, heap_end, heap_ptr);
	return ptr;
}
