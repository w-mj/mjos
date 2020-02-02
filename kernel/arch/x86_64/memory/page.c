#include <memory.h>
#include <delog.h>
#include <list.h>
#include <base.h>
#include <list.h>
#include <asm.h>
#include "early_kmalloc.h"

static FrameEntry *mem_pool = NULL;  // 存放所有页框的数组
static int mem_frame_cnt = 0;  //  总页框数量

static ListEntry mem_free_head;  // 空闲页框列表头
static ListEntry mem_occupied_head;  // 已被占用页框列表头
static int mem_free_cnt = 0;
static int mem_occupied_cnt = 0;

static void init_mem_area(u64 addr, u64 len) {
	u64 addr_end = addr + len;
	// 内存头部和尾部少于一个页的空间都舍弃
	addr_end = ROUND_DOWN(addr_end, PAGESIZE);
	addr     = ROUND_UP(addr, PAGESIZE);
	// 低于16M的空间为DMA区域，不放在内存池中
	if (addr < (16 * (1 << 20))) {
		addr = (16 * (1 << 20));
	}
	len = addr_end - addr;
	int frame_cnt = len / PAGESIZE;  // 页框数
	logi("mem area addr: 0x%x%08x, len: %d, frames: %d", h32(addr), l32(addr), len, frame_cnt);
	if (mem_pool == NULL) {
		mem_pool = (FrameEntry *)early_kmalloc(frame_cnt * sizeof(FrameEntry));
	} else
		// early_kmalloc 只在这里被连续调用，因此多次分配的空间一定是连续的
	{
		early_kmalloc(frame_cnt * sizeof(FrameEntry));
	}
	FrameEntry *mmap = mem_pool + frame_cnt;
	mem_frame_cnt += frame_cnt;
	while (frame_cnt--) {
		mmap->address = (void *)addr;
		addr += PAGESIZE;
		mmap->count = 0;
		mmap->state = AVAILABLE;
		mmap++;
		list_add(&mmap->next, &mem_free_head);
	}
	mem_free_cnt = mem_frame_cnt; // 空闲页框与页框总数保持一致
}

static void rebuild_kernel_page();

void init_page(void *mmap_addr, u64 mmap_length) {
	logi("init page");
	early_kmalloc_init();
	list_init(&mem_free_head);
	list_init(&mem_occupied_head);
	multiboot_memory_map_t *mmap;
	// 遍历内存表
	for (mmap = (multiboot_memory_map_t *)mmap_addr;
	        (u64) mmap < (u64)mmap_addr + mmap_length;
	        mmap = (multiboot_memory_map_t *)((unsigned long ) mmap + mmap->size + sizeof(mmap->size))) {
		// 跳过不可用内存区域
		if (mmap->type != 1) {
			continue;
		}
		// 跳过低于1M的内存区域
		if (mmap->addr < (1 << 20)) {
			continue;
		}
		init_mem_area(mmap->addr, mmap->len);
	}
	logi("frames count: %d", mem_frame_cnt);
	rebuild_kernel_page();
}

static inline void load_page_addr(PageTable *pts[5], int pmloffset[5], u64 addr) {
	_sL(addr);
	_sL(pts[4]);
	for (int i = 4; i > 1; i--) {
		pmloffset[i] = PMLOFFSET(addr, i);
		// _sx(pmloffset[i]);
		pts[i - 1] = (PageTable*)PTENTRYADDR(pts[i][pmloffset[i]]);
	}
	pmloffset[1] = PMLOFFSET(addr, 1);
}
extern u64 heap_end;
extern u64 pml4, pm_end;
void *kernel_pml4;
static void release_early_page() {
	PageTable *pts[5];
	int pts_removes[5] = {0};
	int pmloffset[5];
	pts[4] = (PageTable*)kernel_pml4;
	u64 addr = (u64)(-1);
	u64 need_release = (addr - heap_end) / PAGESIZE + 1;
	_si(need_release);
	load_page_addr(pts, pmloffset, addr);
	int level = 1;
	while (addr >= heap_end) {
		pts[1][pmloffset[1]] = 0;  // 清除页表项
		pts_removes[1] ++;
		while (pmloffset[level] == 0 && level < 4) {
			level++;
			pts[level][pmloffset[level]] = 0;
			pts_removes[level]++;
		}
		/*
		if(level > 1) {
			_si(level);
			repet(4) {_si(_+1);_si(pmloffset[_+1]);}
			repet(4) _si(pts[_+1]);
		}*/
		pmloffset[level]--;
		while (level > 1) {
			pts[level - 1] = (PageTable*)PTENTRYADDR(pts[level][pmloffset[level]]);
			// die();
			pmloffset[level - 1] = 511;
			level--;
		}
		addr -= PAGESIZE;
	}
	for (int i = 1; i <= 4; i++)
		logi("release kernel pml%d %d", i, pts_removes[i]);
}

static void rebuild_kernel_page() {
	logi("rebuild kernel page");
	logi("0x%x%08x", h32(heap_end), l32(heap_end));
	u64 real_heap_end = ABSOLUTE(heap_end);
	_sL(&pml4);
	logi("0x%x%08x", h32((u64)&pml4), l32((u64)&pml4));
	int cnt = 0;
	int pcnt = 0;
	void *early_pms = (void *)ABSOLUTE(&pml4);
	void *early_pme = (void *)ABSOLUTE(&pm_end);
	kernel_pml4 = &pml4;  // 内核4级页表地址
	for (int i = 0; i < mem_frame_cnt; i++) {
		void *addr = mem_pool[i].address;
		// 将内核代码占用的页框标记为已占用
		if ((u64)addr >= (u64)&KERNEL_LMA && addr < (void *)real_heap_end) {

			if (addr >= early_pms && addr < early_pme) {
				// 这个页是内核临时页表页
				pcnt++;
			}
			cnt++;
			mem_pool[i].state = OCCUPIED;
			mem_pool[i].count = 1;
			list_remove(&mem_pool[i].next);
			mem_free_cnt--;
			list_add(&mem_pool[i].next, &mem_occupied_head);
			mem_occupied_cnt++;
		}
	}
	logi("kernel code %d pages", cnt);
	logi("kernel page %d pages", pcnt);
	// release_early_page();
}

