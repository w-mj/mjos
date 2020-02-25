#pragma once
#include <types.h>
#include <boot.h>
#include <list.h>

typedef enum __PageState {
	PG_AVAILABLE,
	PG_KERNEL,  // 从直接映射区间分配页框
	PG_DMA,     // 从DMA区间分配页框
	PG_RESERVE,
	PG_POOL,
	PG_NORMAL,  // 从自由映射区间分配页框
} PageState;

#define NOPAGE (u32)(-1)

typedef struct __PageList {
	pfn_t head;
	pfn_t tail;
} PageList;

typedef struct {
	//int count;  // 引用计数
	//u64 address;  // 起始地址（页框号）
	PageState state;  // 状态
	// ListEntry next;  // 下一个
	pfn_t next;
	pfn_t prev;
	struct {
		u16 obj;   // 在内存池中第一个可用对象的偏移量
		u16 inuse;  // 已分配的对象数目
	};
} Page;

extern Page *page_arr;

void page_init(void *mmp_addr, u64 mmap_length);
pfn_t frame_alloc(PageState state);
void frame_release(pfn_t frame);
pfn_t frames_alloc(int n, PageState state);
void frames_release(int n, pfn_t frame);

// 分配一个内核页，返回页框号，并把页框写入内核页表
pfn_t kernel_page_alloc(PageState state);
pfn_t kernel_pages_alloc(int n, PageState state);
// 通过虚拟地址释放页框
void kernel_page_release(pfn_t);
void kernel_pages_release(pfn_t p, int n);

// 通过页表转换找到虚拟地址对应的物理地址
// PML 第四级页表的物理地址
// vir 虚拟地址
u64 page_translate(u64 pml, u64 vir);

typedef u64 PageTable;

#define PMLOFFSET(x, n) (((x) >> ((n - 1) * 9 + 12)) & 0x1ff)
#define PTENTRYADDR(x) ((x) & (~0xfff))
#define PRESENT(x)     ((x) & 0x01)
#define PT_PS(x)       ((x) & (1 << 7))
#define PAGEPS(x)      ((x) & (0x01 << 7))

#define PML4TE_SHIFT     39                     // page-map level-4 table
#define PDPTE_SHIFT      30                     // page-directory-pointer table
#define PDTE_SHIFT       21                     // page-directory table
#define PTE_SHIFT        12                     // page table

#define MMU_NX          0x8000000000000000UL    // (NX)  No Execute
#define MMU_ADDR        0x000ffffffffff000UL    // addr field
#define MMU_AVL         0x0000000000000e00UL    // AVL
#define MMU_G           0x0000000000000100UL    // (G)   Global
#define MMU_PS          0x0000000000000080UL    // (PS)  Page Size, is it 2M PDE?
#define MMU_D           0x0000000000000040UL    // (D)   Dirty
#define MMU_A           0x0000000000000020UL    // (A)   Accessed
#define MMU_PCD         0x0000000000000010UL    // (PCD) Page-level Cache Disable
#define MMU_PWT         0x0000000000000008UL    // (PWD) Page-level WriteThough
#define MMU_US          0x0000000000000004UL    // (U/S) User Supervisor
#define MMU_RW          0x0000000000000002UL    // (R/W) Read Write
#define MMU_P           0x0000000000000001UL    // (P)   Present
#define MMU_PAT_4K      0x0000000000000080UL    // (PAT) for 4K PTE
#define MMU_PAT_2M      0x0000000000001000UL    // (PAT) for 2M PDE

bool  pglist_is_empty (PageList *list);
void  pglist_push_head(PageList *list, pfn_t page);
void  pglist_push_tail(PageList *list, pfn_t page);
pfn_t pglist_pop_head (PageList *list);
pfn_t pglist_pop_tail (PageList *list);
void  pglist_remove   (PageList *list, pfn_t page);
void  pglist_free_all (PageList *list);

// 在页表中找到一个未被映射的虚拟地址
// pml4:4级页表的物理地址
// ret：空闲的虚拟地址
void *find_virtual_addr(u64 pml4);
