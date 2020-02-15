#pragma once
#include <list.h>

typedef struct __CacheDescriptor {
	ListEntry partial;
	ListEntry next;
	size_t obj_size; // 每个对象的长度
	int partial_cnt; // parial 链表长度
	// int full_slab_obj_cnt;  // 一个slab最多能装的slab数目
	char *name;
} CacheDescriptor;

typedef struct __SlabDescriptor {
	ListEntry next;
	void *obj;  // 第一个可用对象的地址
	int inuse;  // 正在使用的对象数目
} SlabDescriptor;


void mem_pool_init();
void cache_init(CacheDescriptor *cache, char *name, size_t obj_size);
void *cache_obj_alloc(CacheDescriptor *cache);
void cache_obj_release(CacheDescriptor *cache, void *addr);
