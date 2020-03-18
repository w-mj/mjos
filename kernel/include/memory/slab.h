#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <list.h>
#include <memory/page.h>

typedef struct __CacheDescriptor {
	PageList partial;
	ListEntry next;
	size_t obj_size; // 每个对象的长度
	int partial_cnt; // parial 链表长度
	// int full_slab_obj_cnt;  // 一个slab最多能装的slab数目
	char *name;
} CacheDescriptor;

void mem_pool_init();
void cache_init(CacheDescriptor *cache, char *name, size_t obj_size);
void *cache_obj_alloc(CacheDescriptor *cache);
void cache_obj_release(CacheDescriptor *cache, void *addr);

#define NOOBJ (u16)(-1)

#ifdef __cplusplus
}
#endif
