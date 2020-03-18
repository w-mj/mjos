#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <multiboot.h>
#include <arch.h>
#include <process/process.h>
#include <delog.h>
#include <cpu.h>


#define ABSOLUTE(x) (((u64)(x) - ((u64)KERNEL_VMA)) + ((u64)KERNEL_LMA))
#define VIRTUAL(x)  ((void*)((u64)(x) + (((u64)KERNEL_VMA)) - ((u64)KERNEL_LMA)))

#define KERNEL_SIZE ((-1) - KERNEL_VMA + 1)   // 内核空间大小
#define IS_VIRT_KERNEL(x) ((x) >= KERNEL_VMA)
#define IS_VIRT_DMA(x)    ((x) <  16 * (1 << 20))
#define IS_VIRT_PCI(x)    ((x) >= (u64)0xbfee0000 && (x) < (u64)4 * (1 << 30))
#define IS_PHYS_KERNEL(x) (((x) & (KERNEL_VMA - KERNEL_LMA)) == 0)
#define IS_PHYS_DMA(x) IS_VIRT_DMA(x)
#define IS_PHYS_PCI(x) IS_VIRT_PCI(x)

u64 page_translate(u64, u64);
static inline usize virt_to_phys(void * va) {
	u64 v = (u64)va;
	if (IS_VIRT_KERNEL(v)) {
		return ABSOLUTE(v);
	} else if (IS_VIRT_DMA(v)) {
		return v;
	} else if (IS_VIRT_PCI(v)) {
		return v;
	}
	assert(thiscpu_var(current) != NULL);
	return page_translate(thiscpu_var(current)->cr3, v);
}

static inline void * phys_to_virt(usize pa) {
	if (IS_PHYS_DMA(pa)) {
		// DMA 直接映射
		return (void*)pa;
	} else if (IS_PHYS_KERNEL(pa)) {
		// 16M ~ 16M + 2G 内核偏移映射
		return VIRTUAL(pa);
	}
	if (IS_PHYS_PCI(pa)) {
		// 3G ~ 4G PCI 直接映射
		return (void *)pa;
	}
	// 自由页表映射区，不能直接找到虚拟地址
	assert(0);
	return NULL;
}

static inline void *pfn_to_virt(pfn_t pf) {
	return phys_to_virt(pf << PAGEOFFSET);
}

static inline pfn_t virt_to_pfn(void *addr) {
	return virt_to_phys(addr) >> PAGEOFFSET;
}



#ifdef __cplusplus
}
#endif
