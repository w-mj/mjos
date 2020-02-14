#include <memory.h>
#include <delog.h>
#include <list.h>
#include <base.h>
#include <types.h>
#include <string.h>
#include <slab.h>
#include <attribute.h>
#include <asm.h>

typedef struct kmalloc_info_struct {
	char *name;
	size_t obj_size;
} __PACKED KmallocInfo;

const struct kmalloc_info_struct kmalloc_info[] = { 
	{"kmalloc-8",               8},
	{"kmalloc-16",             16},     
	{"kmalloc-32",             32},
	{"kmalloc-64",             64},    
	{"kmalloc-96",             96},
	{"kmalloc-128",           128},
	{"kmalloc-192",           192},     
	{"kmalloc-256",           256},    
	{"kmalloc-512",           512},
	{"kmalloc-1024",         1024},   
	{"kmalloc-2048",         2048},
	{"kmalloc-4096",         4096}, 
	{"kmalloc-8192",         8192},
};
#define KMALLOC_CACHE_LENGTH 13
CacheDescriptor kmalloc_cache[KMALLOC_CACHE_LENGTH];

static inline int kmalloc_cache_index(size_t size) {
	for (int i = 0; i < KMALLOC_CACHE_LENGTH; i++) {
		if (size <= kmalloc_info[i].obj_size)
			return i;
	}
	return -1;
}

void kmalloc_init() {
	for (int i = 1; i < 14; i++) {
		cache_init(&kmalloc_cache[i], kmalloc_info[i].name, kmalloc_info[i].obj_size);
	}
}

void *kmalloc(size_t size) {
	int index = kmalloc_cache_index(size);
	if (index == -1) {
		loge("kmalloc too large %d", size);
		die();
	}
	return 
}
