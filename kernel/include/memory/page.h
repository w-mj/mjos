#pragma once
#include <types.h>
#include <boot.h>
#include <list.h>

typedef enum __PageState {
	PG_AVAILABLE,
	PG_KERNEL,  // 内核页框，不会被换出
	PG_DMA,
	PG_RESERVE,
	PG_POOL,
	PG_NORMAL,
} PageState;

typedef u32 pfn_t;
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

typedef u64 PageTable;

#define PMLOFFSET(x, n) (((x) >> ((n - 1) * 9 + 12)) & 0x1ff)
#define PTENTRYADDR(x) ((x) & (~0xfff))
#define PRESENT(x)     ((x) & 0x01)
#define PT_PS(x)       ((x) & (1 << 7))
#define PAGEPS(x)      ((x) & (0x01 << 7))



bool  pglist_is_empty (PageList *list);
void  pglist_push_head(PageList *list, pfn_t page);
void  pglist_push_tail(PageList *list, pfn_t page);
pfn_t pglist_pop_head (PageList *list);
pfn_t pglist_pop_tail (PageList *list);
void  pglist_remove   (PageList *list, pfn_t page);
void  pglist_free_all (PageList *list);

