#include <memory.h>
#include <list.h>
#include <delog.h>
#include <types.h>
#include <slab.h>

ListEntry cache_list;


static inline int calculate_page(size_t obj_size) {
	// 计算一个slab至少需要多少页
	int t = obj_size + sizeof(SlabDescriptor);
	return (t >> PAGEOFFSET) + ((t & (PAGESIZE - 1)) > 0);
}

void cache_add_slab(CacheDescriptor *cache) {
	int n = calculate_page(cache->obj_size);  // 计算需要多少页
	logd("cache %s add slab, %d pages.", cache->name, n);
	void *mem = kernel_pages_alloc(n);  // 分配页框
	SlabDescriptor *slab = (SlabDescriptor *)mem;
	list_init(&slab->next);
	int obj_cnt = (PAGESIZE - sizeof(SlabDescriptor)) / cache->obj_size;
	_si(obj_cnt);
	slab->obj = (void*)((u64)mem + sizeof(SlabDescriptor));
	slab->inuse = 0;
	list_add(&slab->next, &cache->partial);
	cache->partial_cnt += 1;
	u64 cursor = (u64)slab->obj;
	obj_cnt--;
	while (obj_cnt--) {
		// 建立空闲对象列表
		*(u64*)cursor = cursor + cache->obj_size;
		cursor += cache->obj_size;
	}
	*(u64*)cursor = NULL;  // 最后一个空闲对象的指针设为0
}


void cache_init(CacheDescriptor *cache, char *name, size_t obj_size) {
	list_init(&cache->partial);
	list_init(&cache->next);
	cache->obj_size = obj_size;
	cache->partial_cnt = 0;
	cache->name = name;
	list_add(&cache->next, &cache_list);
}

void mem_pool_init() {
	list_init(&cache_list);
}

void *cache_slab_alloc(SlabDescriptor *slab) {
	void *ret = slab->obj;
	slab->obj = *(void**)slab->obj;
	slab->inuse += 1;
	return ret;
}

void *cache_obj_alloc(CacheDescriptor *cache) {
	if (list_empty(&cache->partial)) {
		// cache中没有空闲的slab，向内核请求页面
		cache_add_slab(cache);
	}
	ListEntry *slab_list_e= cache->partial.next;
	SlabDescriptor *slab = list_entry(slab_list_e, SlabDescriptor, next);
	void *ret = cache_slab_alloc(slab);
	if (slab->obj == NULL) {
		list_remove(&slab->next);
		cache->partial_cnt--;
	}
	return ret;
}

void cache_obj_release(CacheDescriptor *cache, void *addr) {
	SlabDescriptor *slab = (SlabDescriptor*)((u64)addr & (~(PAGESIZE - 1)));
	if (slab->inuse == 1 && cache->partial_cnt > 5) {
		// 再释放这个对象就成为空slab
		logi("release slab");
		list_remove(&slab->next);  // 即使该slab没有挂在partial列表上也没关系
		int n = calculate_page(cache->obj_size);
		kernel_pages_release(slab, n);
		return;
	}
	if (slab->obj == NULL) {
		// 释放这个对象后slab变成partial
		list_add(&slab->next, &cache->partial);
		cache->partial_cnt += 1;
	}
	*(void**)addr = slab->obj;
	slab->obj = addr;
	slab->inuse -= 1;
}

