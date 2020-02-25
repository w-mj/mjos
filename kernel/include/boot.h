#pragma once
#include <types.h>
#include <multiboot.h>
#include <arch.h>


#define ABSOLUTE(x) (((u64)(x) - ((u64)KERNEL_VMA)) + ((u64)KERNEL_LMA))
#define VIRTUAL(x)  ((void*)((u64)(x) + (((u64)KERNEL_VMA)) - ((u64)KERNEL_LMA)))

static inline usize virt_to_phys(void * va) {
	u64 v = (u64)va;
	if (v >= KERNEL_VMA) {
		return ABSOLUTE(v);
	} else if (v < 16 * (1 << 20)) {
		return v;
	} else if (v >= (u64)3 * (1 << 30) && v < (u64)4 * (1 << 30)) {
		return v;
	}
	// TODO: 通过页表转换取物理地址
	return 0;

}

static inline void * phys_to_virt(usize pa) {
	if (pa < 16 * (1 << 20)) {
		// DMA 直接映射
		return (void*)pa;
	} else if (pa < KERNEL_LMA + ((-1) - KERNEL_VMA)) {
		// 16M ~ 16M + 2G 内核偏移映射
		return VIRTUAL(pa);
	}
	if (pa >= (u64)3 * (1 << 30) && pa < (u64)4 * (1 << 30)) {
		// 3G ~ 4G PCI 直接映射
		return (void *)pa;
	}
	// 自由页表映射区，不能直接找到虚拟地址
	return NULL;
}

static inline void *pfn_to_virt(pfn_t pf) {
	return VIRTUAL(pf << PAGEOFFSET);
}

static inline pfn_t virt_to_pfn(void *addr) {
	return ABSOLUTE(addr) >> PAGEOFFSET;
}


