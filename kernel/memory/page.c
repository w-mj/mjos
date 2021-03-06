#include <memory/page.h>
#include <delog.h>
#include <list.h>
#include <base.h>
#include <list.h>
#include <asm.h>
#include <string.h>
#include <arch.h>
#include "early_kmalloc.h"

Page *page_arr = NULL;  // 存放所有页框的数组
static int mem_frame_cnt = 0;  //  总页框数量

//static ListEntry mem_free_head;  // 空闲页框列表头
//static ListEntry mem_occupied_head;  // 已被占用页框列表头
static PageList mem_free_head, mem_occupied_head;
static PageList mem_map_head;  // 可以自由映射的页框
static int mem_free_cnt = 0;
static int mem_occupied_cnt = 0;

bool pglist_is_empty(PageList *list) {
	return list->head == NOPAGE && list->tail == NOPAGE;
}

void pglist_push_head(PageList *list, pfn_t page) {
	page_arr[page].prev = NOPAGE;
	page_arr[page].next = list->head;
	if (list->head != NOPAGE) {
		page_arr[list->head].prev = page;
	}
	list->head = page;
	if (list->tail == NOPAGE) {
		list->tail = page;
	}
}

void pglist_push_tail(PageList *list, pfn_t page) {
	page_arr[page].prev = list->tail;
	page_arr[page].next = NOPAGE;
	if (list->tail != NOPAGE) {
		page_arr[list->tail].next = page;
	}
	list->tail = page;
	if (list->head == NOPAGE) {
		list->head = page;
	}
}

pfn_t pglist_pop_head(PageList *list) {
	pfn_t page = list->head;
	if (page == NOPAGE) {
		return NOPAGE;
	}
	list->head = page_arr[page].next;
	if (list->head == NOPAGE) {
		list->tail = NOPAGE;
	}
	page_arr[page].next = NOPAGE;
	page_arr[page].prev = NOPAGE;
	return page;
}

pfn_t pglist_pop_tail(PageList *list) {
	pfn_t page = list->tail;
	if (page == NOPAGE) {
		return NOPAGE;
	}
	list->tail = page_arr[page].prev;
	if (list->tail == NOPAGE) {
		list->head = NOPAGE;
	}
	page_arr[page].next = NOPAGE;
	page_arr[page].prev = NOPAGE;
	return page;
}

void pglist_remove(PageList *list, pfn_t page) {
	assert(page != NOPAGE);
	Page *frame = &page_arr[page];
	if (frame->prev == NOPAGE) {
		list->head = frame->next;
	} else {
		page_arr[frame->prev].next = frame->next;
	}
	if (frame->next == NOPAGE) {
		list->tail = frame->prev;
	} else {
		page_arr[frame->next].prev = frame->prev;
	}
	frame->prev = NOPAGE;
	frame->next = NOPAGE;
}

void pglist_free_all(PageList *list) {
	pfn_t p;
	while ((p = pglist_pop_head(list)) != NOPAGE) {
		frame_release(p);
	}
}

// 通过页表转换找到虚拟地址对应的物理地址
// PML 第四级页表的物理地址
// vir 虚拟地址
u64 page_translate(u64 pml, u64 vir) {
	int level = 4;
	while (level >= 1) {
		u64 *pml_addr = (u64 *)VIRTUAL(pml);
		u16 pmli = PMLOFFSET(vir, level);
		u64 pmle = pml_addr[pmli];
		if (!PRESENT(pmle)) {
			loge("page not present at level %d", level);
			die();
		}
		pml = PTENTRYADDR(pmle);
//		_sL(pmle);
		level--;
	}
	return pml | (vir & 0xfff);
}

extern u64 heap_end;
extern u64 pml4, pm_end;
u64 kernel_pml4; // 内核4级页表物理地址

// 分配一个空闲页框，返回页框号
static int frame_alloc_cnt = 0;
pfn_t frame_alloc(PageState state) {
	if (mem_free_cnt == 0) {
		loge("No remain frame");
		return 0;
	}
	frame_alloc_cnt++;
	pfn_t free_frame = NOPAGE;
	assert(state == PG_KERNEL || state == PG_NORMAL || state == PG_POOL);
	if (state == PG_KERNEL || state == PG_POOL) {
		free_frame = pglist_pop_tail(&mem_free_head);
	} else if (state == PG_NORMAL) {
		free_frame = pglist_pop_tail(&mem_map_head);
	}
	assert(free_frame != NOPAGE);
	// _sx(free_frame);
	Page *frame_entry = &page_arr[free_frame];
	// frame_entry->count++;
	assert(frame_entry->state == PG_AVAILABLE);
	frame_entry->state = state;
	mem_free_cnt--;
	if (state == PG_KERNEL || state == PG_NORMAL) {
		mem_occupied_cnt++;
		pglist_push_head(&mem_occupied_head, free_frame);
	}
	return free_frame;
}

pfn_t frames_alloc(int n, PageState state) {
	pfn_t a = frame_alloc(state);
	n--;
	while (n--) {
		frame_alloc(state);
	}
	return a;
}

void frame_release(pfn_t fn) {
	Page *frame = &page_arr[fn];
	if (frame->state == PG_KERNEL) {
		mem_occupied_cnt--;
		pglist_remove(&mem_occupied_head, fn);
		frame->state = PG_AVAILABLE;
		pglist_push_head(&mem_free_head, fn);
	} else if (frame->state == PG_NORMAL) {
		mem_occupied_cnt--;
		pglist_remove(&mem_occupied_head, fn);
		frame->state = PG_AVAILABLE;
		pglist_push_head(&mem_map_head, fn);
	} else if (frame->state == PG_POOL) {
		frame->state = PG_AVAILABLE;
		pglist_push_head(&mem_free_head, fn);
	}
	// list_remove(&frame->next);
	// list_add(&frame->next, &mem_free_head);
	mem_free_cnt++;
}

void frames_release(int n, pfn_t fn) {
	while (n--) {
		frame_release(fn);
		fn++;
	}
}

static void page_table_set_entry(u64 pmltop, u64 page_table_entry, u64 value, bool auto_alloc) {
	// if (pmltop != kernel_pml4) {
	// 	_sL(pmltop);
	// 	_sL(page_table_entry);
	// 	_sL(value);
	// }

	int level = 4;
	u64 *pml = (u64 *)pmltop;
	while (level > 1) {
		pml = (u64 *)VIRTUAL(pml); // 物理页框号转换为虚拟地址
		u16 pmli = PMLOFFSET(page_table_entry, level);
		u64 pmle = pml[pmli];
		if (!PRESENT(pmle)) {
			if (!auto_alloc) {
				loge("page_table_set_entry pml%d[%d] is not present", level, pmli);
				return;
			} else {
				// if (pmltop != kernel_pml4) {
				// 	_sL(pml);
				// 	_si(pmli);
				// 	_sL(pmle);
				// 	_si(level);
				// }

				// 自动分配页框
				pmle = pml[pmli] = (frame_alloc(PG_KERNEL) << PAGEOFFSET)|MMU_P| MMU_US| MMU_RW|MMU_PCD| MMU_PWT ;
				// logd("auto alloc frame %llx %llx", VIRTUAL(pmle), pmle);
								page_table_set_entry(pmltop, (u64)VIRTUAL(pmle), pmle, true);
				heap_end += PAGESIZE;
			}
		}
		pml = (u64 *)PTENTRYADDR(pmle);
		level--;
	}
	pml = (u64 *)VIRTUAL(pml); // 物理页框号转换为虚拟地址
	u16 pml1i = PMLOFFSET(page_table_entry, 1);
	pml[pml1i] = value | MMU_PCD| MMU_PWT;
}

static inline u64 mk_virtual(int *frames) {
	return ((u64)frames[4] << PML4TE_SHIFT) + \
	       ((u64)frames[3] << PDPTE_SHIFT)  + \
	       ((u64)frames[2] << PDTE_SHIFT)   + \
	       ((u64)frames[1] << PTE_SHIFT);
}
static inline void mk_frame(int *frames, u64 virtual) {
	frames[4] = (int)(virtual >> PML4TE_SHIFT) & 0x1ff;
	frames[3] = (int)(virtual >> PDPTE_SHIFT) & 0x1ff;
	frames[2] = (int)(virtual >> PDTE_SHIFT) & 0x1ff;
	frames[1] = (int)(virtual >> PTE_SHIFT) & 0x1ff;
}
// 找到一个空闲的虚拟地址
// 硬件映射地址已经被标记为直接映射，可以大胆遍历
void *find_virtual_addr(u64 pml4, u64 above) {
	int level = 4;
	int pml_cursor[5] = {0};
	mk_frame(pml_cursor, above);
	u64 *pml_addr[5] = {0};
	pml_addr[level] = (u64*)phys_to_virt(pml4);
	do {
		// 初始化4级页表的虚拟地址
		while (level > 1) {
			// 当前等级的页表项
			u64 entry = pml_addr[level][pml_cursor[level]];
			if (! (entry & MMU_P)) {
				u64 addr = mk_virtual(pml_cursor);
				if (addr < KERNEL_VMA)
					return (void*)addr;
				return NULL;
			}
			pml_addr[level - 1] = (u64*)phys_to_virt(entry & MMU_ADDR);
			level--;
		}
		// 遍历最低级页表，此处level == 1
		while (pml_cursor[level] < 512) {
			u64 entry = pml_addr[level][pml_cursor[level]];
			if (! (entry & MMU_P)) {
				u64 addr = mk_virtual(pml_cursor);
				if (addr < KERNEL_VMA)
					return (void*)addr;
				return NULL;
			}
			pml_cursor[level]++;
		}
		while (level < 4 && pml_cursor[level] == 512) {
			// 向上升级
			pml_cursor[level] = 0;
			level++;
			pml_cursor[level]++;
			if (level == 4 && pml_cursor[4] == 512) {
				return NULL;
			}
		}
	} while (true);
	return NULL;
}



static void write_new_kernel_page_table() {
	u64 newmp = frame_alloc(PG_KERNEL) << PAGEOFFSET;  // 新的四级页表物理地址
	kernel_pml4 = newmp;
	_sL(newmp);
	memset((u64 *)VIRTUAL(newmp), 0, PAGESIZE);
	_sL(VIRTUAL(newmp));
	u64 addr = 0;
	addr = 0;
	// 低16M
    // 0xffffffff80000000 - 0xffffffff81000000
	while (addr < 16 * (1 << 20)) {
		page_table_set_entry(newmp, (u64)VIRTUAL(addr), addr| MMU_P| MMU_RW| MMU_US| MMU_PCD| MMU_PWT, true);
		addr += PAGESIZE;
	}
	addr = 0xbfee0000;  // 3G
	// 3G - 4G　PCI映射区直接映射
	while (addr < (u64)4 * (1 << 30)) {
		page_table_set_entry(newmp, addr, addr| MMU_P |MMU_RW| MMU_US| MMU_PCD| MMU_PWT, true);
		addr += PAGESIZE;
	}
	// 构建内核代码数据页表
	// 0xffffffff81000000 -> 0x0000000001000000
	addr = (u64)KERNEL_VMA;
	while (addr < heap_end) {
		page_table_set_entry(newmp, addr,  ABSOLUTE(addr & (~(0xfff))) + 7, true);
		addr += PAGESIZE;
	}
	page_table_set_entry(newmp, (u64)VIRTUAL(newmp), newmp + 7, true);
	logd("new kernel page table occupies %d frames", frame_alloc_cnt);
	write_cr3(newmp);
	// ASM("int $3");
	logd("load new page table finish");
}

static void rebuild_kernel_page() {
	logi("rebuild kernel page");
	// logi("0x%x%08x", h32(heap_end), l32(heap_end));
	// u64 real_heap_end = ABSOLUTE(heap_end);
	_sL(&pml4);
	// logi("0x%x%08x", h32((u64)&pml4), l32((u64)&pml4));
	int cnt = 0;
	int pcnt = 0;
	// 临时内核页表地址
	void *early_pms = (&pml4);
	void *early_pme = (&pm_end);
	kernel_pml4 = (u64)early_pms;  // 内核4级页表地址
	void *addr;
	assert(mem_free_head.tail != NOPAGE);
	for (addr = (void *)KERNEL_VMA; addr < (void *)heap_end; addr += PAGESIZE) {
		if (addr >= early_pms && addr < early_pme) {
			// 这个页是内核临时页表页
			pcnt++;
		}
		// _sL(addr);
		cnt++;
		pfn_t i = ABSOLUTE(addr) >> PAGEOFFSET;
		if (mem_free_head.tail == NOPAGE) {
			_si(cnt);
			_si(i);
			_sx(page_arr[i].prev);
			_sx(page_arr[i].next);
			_si(page_arr[i].state);
			_sL(i << PAGEOFFSET);
			die();
		}
		page_arr[i].state = PG_KERNEL;
		// page_arr[i].count = 1;
		// list_remove(&page_arr[i].next);
		pglist_remove(&mem_free_head, i);
		mem_free_cnt--;
		// list_add(&page_arr[i].next, &mem_occupied_head);
		pglist_push_head(&mem_occupied_head, i);
		mem_occupied_cnt++;
	}
	logi("kernel code %d pages", cnt);
	logi("kernel page %d pages", pcnt);
	// 写入新内核页表
	write_new_kernel_page_table();
	// 释放临时页表
	cnt = 0;
	addr = &pml4;
	while (addr < (void *)&pm_end) {
		page_table_set_entry(kernel_pml4, (u64)addr, 0, false);
		pfn_t page_index = ABSOLUTE(addr) >> PAGEOFFSET;
		pglist_remove(&mem_occupied_head, page_index);
		mem_occupied_cnt -= 1;
		page_arr[page_index].state = PG_AVAILABLE;
		addr += PAGESIZE;
		cnt++;
	}
	/*
	ListEntry *cursor = mem_occupied_head.next;
	while (cursor != &mem_occupied_head) {
		Page *frame = list_entry(cursor, Page, next);
		cursor = cursor->next;
		if (frame->address >= (u64) early_pms && frame->address < (u64) early_pme) {
			list_remove(&frame->next);
		}
	}
	*/
	logi("release %d early kernel frames.", cnt);
}
void page_init(void *mmap_addr, u64 mmap_length) {
	logi("init page");
	//list_init(&mem_free_head);
	//ist_init(&mem_occupied_head);
	mem_free_head.head = NOPAGE;
	mem_free_head.tail = NOPAGE;
	mem_occupied_head.head = NOPAGE;
	mem_occupied_head.tail = NOPAGE;
	mem_map_head.head = NOPAGE;
	mem_map_head.tail = NOPAGE;
	mem_free_cnt = 0;
	mem_occupied_cnt = 0;
	multiboot_memory_map_t *mmap;
	u64 mem_size = 0;
	for (mmap = (multiboot_memory_map_t *)mmap_addr;
	        (u64) mmap < (u64)mmap_addr + mmap_length;
	        mmap = (multiboot_memory_map_t *)((unsigned long ) mmap + mmap->size + sizeof(mmap->size))) {
		mem_size = mmap->addr + mmap->len;
	}
	logi("Detected memory size %d", mem_size);
	mem_frame_cnt = ROUND_UP(mem_size, 4096) / PAGESIZE;
	_si(mem_frame_cnt);
	page_arr = (Page *)early_kmalloc(mem_frame_cnt * sizeof(Page)); // 为所有页框分配内存
	// for (int i = 0; i < mem_frame_cnt; i++)
	// 	page_arr[i].state = RESERVE;
	int ava_frame = 0;
	u64 last_end = 0;

	int kernel_frame = 0;
	int map_frame = 0;
	// 再次遍历内存表，初始化页框表
	for (mmap = (multiboot_memory_map_t *)mmap_addr;
	        (u64) mmap < (u64)mmap_addr + mmap_length;
	        mmap = (multiboot_memory_map_t *)((unsigned long ) mmap + mmap->size + sizeof(mmap->size))) {
		// init_mem_area(mmap->addr, mmap->len, mmap->type);
		// 填充在内存表中没有表现出来的空洞
		logd("last end: %llx, addr: %llx, len %llx", last_end, mmap->addr, mmap->len);
		while (last_end < mmap->addr) {
			page_arr[last_end >> PAGEOFFSET].state = PG_RESERVE;
			last_end += PAGESIZE;
		}
		last_end = mmap->addr + mmap->len;
		u64 addr_end, addr;
		if (mmap->type == 1) {
			// 可用内存尽量小，不可用区域尽量大
			addr_end = ROUND_DOWN(mmap->addr + mmap->len, PAGESIZE);
			addr   = ROUND_UP(mmap->addr, PAGESIZE);
		} else {
			addr_end = ROUND_UP(mmap->addr + mmap->len, PAGESIZE);
			addr   = ROUND_DOWN(mmap->addr, PAGESIZE);
		}
		int frame_cnt = (addr_end - addr) / PAGESIZE;
		int frame_start = (addr) / PAGESIZE;
		// _si(frame_start);
		// _si(frame_cnt + frame_start);
		for (int i = frame_start; i < frame_cnt + frame_start; i++) {
			page_arr[i].next = NOPAGE;
			page_arr[i].prev = NOPAGE;
			// list_init(&page_arr[i].next);
			// page_arr[i].count = 0;
			// page_arr[i].address = i * PAGESIZE;
			if (mmap->type == 2) {
				page_arr[i].state = PG_RESERVE;
			} else if (i * PAGESIZE < 16 * (1 << 20)) {
				page_arr[i].state = PG_DMA;
			} else {
				page_arr[i].state = PG_AVAILABLE;
				ava_frame++;
				if (IS_PHYS_KERNEL((u64)i << PAGEOFFSET)) {
					assert(((u64)i << PAGEOFFSET) < (u64)2 * (1 << 30));
					pglist_push_head(&mem_free_head, i);
					kernel_frame++;
				} else {
					pglist_push_head(&mem_map_head, i);
					map_frame++;
				}
				mem_free_cnt++;
				// list_add(&page_arr[i].next, &mem_free_head);
			}
		}
	}
	_si(kernel_frame);
	_si(map_frame);
	_si(mem_free_head.head);
	_si(mem_free_head.tail);
	logi("Build frame pool %d available, %d frames", ava_frame, mem_frame_cnt);
	early_kmalloc_depercated();  //  停用early_kmalloc
	rebuild_kernel_page();
}

pfn_t kernel_pages_alloc(int n, PageState state) {
	assert(state == PG_KERNEL || state == PG_POOL);
	// 分配n个页面，此处应该加锁，后期使用buddy优化
	pfn_t p = frame_alloc(state);
	// logd("alloc kernel %d page pfn:%d", n, p);
	u64 psys = p << PAGEOFFSET;
	page_table_set_entry(kernel_pml4, (u64)VIRTUAL(psys), psys + 7, true);
	n -= 1;
	while (n--) {
		// Page *frame = &page_arr[frame_alloc()];
		pfn_t t = frame_alloc(state);
		psys = t << PAGEOFFSET;
		page_table_set_entry(kernel_pml4, (u64)VIRTUAL(psys), psys + 7, true);
	}
	return p;
}

void kernel_pages_release(pfn_t page, int n) {
	logi("release kernel %d page pfn: %d", n, page);
	while (n--) {
		void *addr = VIRTUAL(page << PAGEOFFSET);
		// Page *f = page_arr + frame;
		frame_release(page);
		page_table_set_entry(kernel_pml4, (u64)addr, 0, false);
		page += 1;
	}
}

pfn_t kernel_page_alloc(PageState state) {
	return kernel_pages_alloc(1, state);
}

void kernel_page_release(pfn_t page) {
	kernel_pages_release(page, 1);
}

void *normal_pages_alloc(u64 pml4, u64 above, int n) {
    void *vir = find_virtual_addr(pml4, above);
    void *ret = vir;
    while (n--) {
        pfn_t t = frame_alloc(PG_NORMAL);
        page_table_set_entry(pml4, (u64)vir, ((u64)t << PAGEOFFSET) | MMU_P| MMU_RW| MMU_US, true);
        vir += PAGESIZE;
    }
    return ret;
}

void *normal_page_alloc(pfn_t *pn, u64 pml4, u64 above) {
	pfn_t t = frame_alloc(PG_NORMAL);
	void *vir = find_virtual_addr(pml4, above);
	_sL(vir);
	// assert(kernel_pml4 == read_cr3());
	page_table_set_entry(pml4, (u64)vir, ((u64)t << PAGEOFFSET) | MMU_P | MMU_RW | MMU_US, true);
	if (pn != NULL)
		*pn = t;
	return vir;
}

void normal_pages_release(void *addr, u64 pml4, int n) {
    while (n--) {
        normal_page_release(addr, pml4);
        addr += PAGESIZE;
    }
}

void normal_page_release(void *addr, u64 pml4) {
	pfn_t fn = virt_to_pfn(addr);
	page_table_set_entry(pml4, (u64)addr, 0, false);
	frame_release(fn);
}
static inline u64 mk_page_entry(pfn_t frame, u64 flags) {
	return ((u64)frame << PAGEOFFSET) | flags;
}

void copy_page(u64 top, u64 to, u64 from, int level);
// 创建用户页表
// 内核空间从0xffffffff81000000 - 全F共有2G空间
//         0x000000007fffffff
//             ffffffff8565dff0
// 占用2^19个页面，2^10个二级页表项，2个三级页表项
// 即占用四级页表的最后一项和三级页表的最后两项
// PCI 空间从bfee0000到4G
// 从[0,2,511,224]到[0,3,511,511]
u64 create_user_page(ProcessDescriptor *process) {
	pfn_t pml          = kernel_page_alloc(PG_KERNEL);
    u64  pml_phy       = pml << PAGEOFFSET;
//	copy_page(process->parent->cr3, pml_phy, process->parent->cr3, 4);
//    return pml_phy;

    pfn_t pml_0        = kernel_page_alloc(PG_KERNEL);
    pfn_t pml_511      = kernel_page_alloc(PG_KERNEL);
    // pfn_t pml_0_0      = kernel_page_alloc(PG_KERNEL);
    pfn_t pml_0_2      = kernel_page_alloc(PG_KERNEL);
    pfn_t pml_0_2_511  = kernel_page_alloc(PG_KERNEL);

    add_to_mem_list(process, pml, VIRTUAL(pml));
    add_to_mem_list(process, pml_0, VIRTUAL(pml));
    add_to_mem_list(process, pml_511, VIRTUAL(pml));
//    add_to_mem_list(process, pml_0_0, VIRTUAL(pml));
    add_to_mem_list(process, pml_0_2, VIRTUAL(pml));
    add_to_mem_list(process, pml_0_2_511, VIRTUAL(pml));

	u64  pml_0_phy       = pml_0 << PAGEOFFSET;
    u64  pml_511_phy     = pml_511 << PAGEOFFSET;
//	u64  pml_0_0_phy     = pml_0_0 << PAGEOFFSET;
    u64  pml_0_2_phy     = pml_0_2 << PAGEOFFSET;
    u64  pml_0_2_511_phy = pml_0_2_511 << PAGEOFFSET;
	// _sL(pml4_phy);
	// _sL(pml3_phy);
	u64 *pml_vir         = (u64*)phys_to_virt(pml_phy);
    u64 *pml_0_vir       = (u64*)phys_to_virt(pml_0_phy);
    u64 *pml_511_vir     = (u64*)phys_to_virt(pml_511_phy);
//    u64 *pml_0_0_vir     = (u64*)phys_to_virt(pml_0_0_phy);
    u64 *pml_0_2_vir     = (u64*)phys_to_virt(pml_0_2_phy);
    u64 *pml_0_2_511_vir = (u64*)phys_to_virt(pml_0_2_511_phy);

	memset(pml_vir        , 0, PAGESIZE);
    memset(pml_0_vir      , 0, PAGESIZE);
    memset(pml_511_vir    , 0, PAGESIZE);
//    memset(pml_0_0_vir    , 0, PAGESIZE);
    memset(pml_0_2_vir    , 0, PAGESIZE);
    memset(pml_0_2_511_vir, 0, PAGESIZE);

	u64 *kernel_pml_vir         = (u64*)phys_to_virt(kernel_pml4);
	u64  kernel_pml_0_phy       = kernel_pml_vir[0] & MMU_ADDR;
	u64 *kernel_pml_0_vir       = (u64*)phys_to_virt(kernel_pml_0_phy);
    u64  kernel_pml_511_phy     = kernel_pml_vir[511] & MMU_ADDR;
    u64 *kernel_pml_511_vir     = (u64*)phys_to_virt(kernel_pml_511_phy);
//	u64  kernel_pml_0_0_phy     = kernel_pml_0_vir[0] & MMU_ADDR;
//	u64 *kernel_pml_0_0_vir     = (u64*)phys_to_virt(kernel_pml_0_0_phy);
	u64  kernel_pml_0_2_phy     = kernel_pml_0_vir[2] & MMU_ADDR;
	u64 *kernel_pml_0_2_vir     = (u64*)phys_to_virt(kernel_pml_0_2_phy);
	u64  kernel_pml_0_2_511_phy = kernel_pml_0_2_vir[511] & MMU_ADDR;
	u64 *kernel_pml_0_2_511_vir = (u64*)phys_to_virt(kernel_pml_0_2_511_phy);

    pml_vir[0]     = mk_page_entry(pml_0, MMU_US | MMU_P | MMU_RW);
//    pml_0_vir[0]   = mk_page_entry(pml_0_0, MMU_US | MMU_P | MMU_RW);
//    pml_0_0_vir[0] = kernel_pml_0_0_vir[0];
//    pml_0_0_vir[1] = kernel_pml_0_0_vir[1];
//    pml_0_0_vir[2] = kernel_pml_0_0_vir[2];
//    pml_0_0_vir[3] = kernel_pml_0_0_vir[3];
//    pml_0_0_vir[4] = kernel_pml_0_0_vir[4];
//    pml_0_0_vir[5] = kernel_pml_0_0_vir[5];
//    pml_0_0_vir[6] = kernel_pml_0_0_vir[6];
//    pml_0_0_vir[7] = kernel_pml_0_0_vir[7];
    pml_0_vir[3]   = kernel_pml_0_vir[3];
    pml_0_vir[2]   = mk_page_entry(pml_0_2, MMU_US | MMU_P | MMU_RW);
    pml_0_2_vir[511] = mk_page_entry(pml_0_2_511, MMU_US | MMU_P | MMU_RW);
    for (int i = 224; i < 512; i++) {
        pml_0_2_511_vir[i] = kernel_pml_0_2_511_vir[i];
    }
    pml_vir[511]     = mk_page_entry(pml_511, MMU_US | MMU_P | MMU_RW);
    pml_511_vir[510] = kernel_pml_511_vir[510];
    pml_511_vir[511] = kernel_pml_511_vir[511];
	// _sL(pml4_vir);
	// _sL(pml4_vir[511]);
	// _sL(pml3_vir[510]);
	// _sL(pml3_vir[511]);
	// _pos();
	page_table_set_entry(pml_phy, (u64)pml_vir, pml_phy + 7, true);
	// _pos();
	page_table_set_entry(pml_phy, (u64)pml_511_vir, pml_511_phy + 7, true);
    page_table_set_entry(pml_phy, (u64)pml_0_vir, pml_0_phy + 7, true);
//    page_table_set_entry(pml_phy, (u64)pml_0_0_vir, pml_0_0_phy + 7, true);
    page_table_set_entry(pml_phy, (u64)pml_0_2_511_vir, pml_0_2_511_phy + 7, true);
    page_table_set_entry(pml_phy, (u64)pml_0_2_vir, pml_0_2_phy + 7, true);
	// _pos();
	return pml_phy;
}

void copy_page(u64 top, u64 to, u64 from, int level) {
    u64 *to_vir = (u64*)phys_to_virt(to);
    u64 *from_vir =(u64*) phys_to_virt(from);
    for (int i = 0; i < 512; i++) {
        if (! PRESENT(from_vir[i]))
            continue;
        if (level == 1) {
            to_vir[i] = from_vir[i];
        } else {
            pfn_t new_page = kernel_page_alloc(PG_KERNEL);
            u64 new_phy = new_page << PAGEOFFSET;
            to_vir[i] = mk_page_entry(new_page, MMU_US| MMU_P| MMU_RW);
            page_table_set_entry(top, (u64)phys_to_virt(new_phy), new_phy | MMU_US| MMU_P | MMU_RW, true);
            copy_page(top, new_phy,from_vir[i] & MMU_ADDR, level - 1);
        }
    }
}

