#pragma once
#include <types.h>
#include <multiboot.h>
#include <arch.h>


#define ABSOLUTE(x) (((u64)(x) - ((u64)KERNEL_VMA)) + ((u64)KERNEL_LMA))
#define VIRTUAL(x)  ((void*)((u64)(x) + (((u64)KERNEL_VMA)) - ((u64)KERNEL_LMA)))

static inline usize virt_to_phys(void * va) {
	return ABSOLUTE(va);
	/*
    if ((usize) va >= KERNEL_VMA) {
        return (usize) va - KERNEL_VMA + KERNEL_LMA;
    } else if (((u8 *) MAPPED_ADDR <= (u8 *) va) &&
               ((u8 *) va < (u8 *) MAPPED_ADDR + MAPPED_SIZE)) {
        return (usize) va - MAPPED_ADDR;
    }
    return NOADDR;
	*/
}

static inline void * phys_to_virt(usize pa) {
	// 16M以下直接映射
	if (pa < 16 * (1 << 20))
		return (void*)pa;
	return VIRTUAL(pa);
	/*
    if (pa < MAPPED_SIZE) {
        return (void *) (pa + MAPPED_ADDR);
    }
    return NULL;
	*/
}

static inline void *pfn_to_virt(pfn_t pf) {
	return VIRTUAL(pf << PAGEOFFSET);
}

static inline pfn_t virt_to_pfn(void *addr) {
	return ABSOLUTE(addr) >> PAGEOFFSET;
}


