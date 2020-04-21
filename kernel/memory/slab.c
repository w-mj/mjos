#include <list.h>
#include <delog.h>
#include <types.h>
#include <memory/slab.h>
#include <memory/page.h>

ListEntry cache_list;


static inline int calculate_page(size_t obj_size) {
	// 计算一个slab至少需要多少页
	return ROUND_UP(obj_size, PAGESIZE) >> PAGEOFFSET;
}

void cache_add_slab(CacheDescriptor *cache) {
	int n = calculate_page(cache->obj_size);  // 计算需要多少页
	pfn_t page = kernel_pages_alloc(n, PG_POOL);  // 分配页框
    logd("cache %s add slab, %d pages pfn %d.", cache->name, n, page);
	Page *frame = &page_arr[page];
	assert(frame->state == PG_POOL);
	int obj_cnt = PAGESIZE * n / cache->obj_size;
	// _si(obj_cnt);
	frame->obj = 0;
	// slab->obj = (void*)((u64)mem + sizeof(SlabDescriptor));
	frame->inuse = 0;
	pglist_push_tail(&cache->partial, page);
	cache->partial_cnt += 1;
	u8 *addr = (u8*)VIRTUAL(page << PAGEOFFSET);
	for (u16 i = 0; i < obj_cnt - 1; i++) {
		*(u16*)(addr + i * cache->obj_size) = (i + 1) * cache->obj_size;
	}
	*(u16*)(addr + (obj_cnt - 1) * cache->obj_size) = NOOBJ;
}


void cache_init(CacheDescriptor *cache, char *name, size_t obj_size) {
	// list_init(&cache->partial);
	logd("cache init %s, obj size: %d", name, obj_size);
	cache->partial.head = NOPAGE;
	cache->partial.tail = NOPAGE;
	list_init(&cache->next);
	cache->obj_size = obj_size;
	cache->partial_cnt = 0;
	cache->name = name;
	list_add(&cache->next, &cache_list);
}

void mem_pool_init() {
	list_init(&cache_list);
}

void *cache_slab_alloc(pfn_t slab) {
	Page *page = &page_arr[slab];
	void *ret = VIRTUAL(slab << PAGEOFFSET) + page->obj;
	page->obj = *(u16*)ret;
	page->inuse += 1;
	return ret;
}

void *cache_obj_alloc(CacheDescriptor *cache) {
	if (pglist_is_empty(&cache->partial)) {
		// cache中没有空闲的slab，向内核请求页面
		cache_add_slab(cache);
	}
	pfn_t slab = cache->partial.head;
	Page *page = &page_arr[slab];
    if (slab == 22154) {
        slab = 22154;
    }
	// ListEntry *slab_list_e= cache->partial.next;
	// SlabDescriptor *slab = list_entry(slab_list_e, SlabDescriptor, next);
	void *ret = cache_slab_alloc(slab);
	if (page->obj == NOOBJ) {
		// slab已经全部分配完
		logd("slab %d is now empty", slab);
		pglist_remove(&cache->partial, slab);
		cache->partial_cnt--;
	}
	return ret;
}

void cache_obj_release(CacheDescriptor *cache, void *addr) {
	pfn_t slab = ABSOLUTE(addr) >> PAGEOFFSET;
	Page *page= &page_arr[slab];
	// _si(slab->inuse);
	// _si(cache->partial_cnt);
	if (page->inuse == 1 && cache->partial_cnt >= 5) {
		// 再释放这个对象就成为空slab
		logi("release slab");
		// 释放空slab
		// 此时该slab可能在partial列表上，也可能是游离的
		if (page->prev == NOPAGE && page->next == NOPAGE) {
			// 如果slab前后都没有页，说明是游离的slab，不用从partial中移除
		} else {
			// 从partial中移除slab
			pglist_remove(&cache->partial, slab);
		}
		int n = calculate_page(cache->obj_size);
		kernel_pages_release(slab, n);
		logd("cache %s release %d pages pfn: %d", cache->name, n, page);
		return;
	}
	if (page->obj == NOOBJ) {
		// 释放这个对象后slab变成partial，把slab加入partial列表中
		pglist_push_tail(&cache->partial, slab);
		cache->partial_cnt += 1;
	}
	*(u16*)addr = page->obj;
	page->obj = LNclr((u64)addr, PAGEOFFSET);
	page->inuse -= 1;
}

