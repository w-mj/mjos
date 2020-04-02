#include <string.h>
#include <list.h>
#include <base.h>
#include <delog.h>
#include <asm.h>
#include <cpu.h>
#include <memory/page.h>
#include <memory/kmalloc.h>
#include <process/scheduler.h>
#include <process/process.h>
#include <fs/syscall_handler.h>
#include <syscall.h>
#include "process/process_helper.hpp"

static u8 *pid_bitmap = NULL;
static ListEntry process_list;

void process_init() {
	logd("init process");
	pid_bitmap = (u8*)pfn_to_virt(kernel_page_alloc(PG_KERNEL));
	memset(pid_bitmap, 0, PAGESIZE);
	list_init(&process_list);
}

static u16 find_usable_pid() {
	u16 pid = 0;
	while(pid < 32767) {
		if (!chk_bitmap(pid_bitmap, pid)) {
			set_bitmap(pid_bitmap, pid);
			return pid;
		}
		pid++;
	}
	return (u16)-1;
}

static bool process_mem_chk(ListEntry *entry, void *pfn) {
    MemList *mem = list_entry(entry, MemList, next);
    return mem->page == *(pfn_t*)pfn;
}

void add_to_mem_list(ProcessDescriptor *process, pfn_t pfn, void *addr) {
    MemList *mem = (MemList *)kmalloc_s(sizeof(MemList));
    list_init(&mem->next);
    mem->page = pfn;
    mem->addr = addr;
    list_add(&mem->next, &process->mem_list);
}

void remove_from_mem_list(ProcessDescriptor *process, pfn_t pfn) {
    ListEntry *e = list_find(&process->mem_list, &pfn, process_mem_chk);
    if (e == NULL) {
        loge("pfn %d is not in process %d's mem list", pfn, process->pid);
        return;
    }
    MemList *mem = list_entry(e, MemList, next);
    kfree_s(sizeof(MemList), mem);
}

ThreadDescriptor *create_thread(ProcessDescriptor *process, void *main) {
	logd("create thread %d for pid %d", process->threads_cnt, process->pid);
	ThreadDescriptor *thread = (ThreadDescriptor*) kmalloc_s(sizeof(ThreadDescriptor));
	_sL(process->cr3);
	pfn_t pfn;
	void *stack = normal_page_alloc(&pfn, process->cr3, ((u64)2 << 30));
	add_to_mem_list(process, pfn, stack);
	pfn = kernel_page_alloc(PG_KERNEL);
	void *sp0 = pfn_to_virt(pfn);
    add_to_mem_list(process, pfn, sp0);
//	u64* page = phys_to_virt(process->cr3);
//	_sL(page[0]);
//	_sL(page[511]);
	thread->rsp = stack + PAGESIZE;
	thread->stack = thread->rsp;
	thread->stack_length = 1;
	thread->rsp0 = sp0  + PAGESIZE;
	thread->cr3 = process->cr3;
    thread->tid = process->threads_cnt;
    thread->process = process;
	if (process->type == PROCESS_KERNEL) {
		thread->rsp = init_kernel_thread_stack(thread->rsp, main);
	} else {
	    thread->rsp = init_user_thread_stack(thread, main);
		// u64 kcr3 = read_cr3();
		// u64 ucr3 = process->cr3;
		// u64 ip = 0xffffffff81016a5d;
		// _sL(page_translate(kcr3, ip));
		// _sL(page_translate(ucr3, ip));
		// write_cr3(ucr3);  // 切换成用户页表
		// thread->rsp = init_thread_stack(stack, main, USER_CODE_DEC, USER_DATA_DEC);
		// write_cr3(kcr3);
	}
	if (process->shared_mem == NULL) {
	    process->shared_mem = (char *)sp0;   // 内核栈的低部分是共享内存空间
	}

	process->threads_cnt++;
	list_init(&thread->next);
	list_init(&thread->sibling);
	list_add(&thread->sibling, &process->threads);
	add_thread_to_running(thread);
	return thread;
}

pid_t create_process(ProcessDescriptor *parent, ProcessType type, void *main, CreateProcessFrom from) {
	pid_t pid = find_usable_pid();
	if (pid == (u16)-1) {
		loge("no usable pid.");
		die();
	}
	logd("create process %d", pid);

	ProcessDescriptor *pd = (ProcessDescriptor*)kmalloc_s(sizeof(ProcessDescriptor));
	pd->pid = pid;
	pd->type = type;
	pd->parent = parent;
	pd->threads_cnt = 0;
	pd->shared_mem = NULL;
	pd->shared_mem_write_lock = SPIN_INIT;
	pd->shared_mem_read_lock  = SPIN_INIT;
	pd->linear_start = (void*)0x400000;  // x64 起始地址
	pd->linear_end = pd->linear_start;
	memset(pd->fds, 0, sizeof(pd->fds));
	raw_spin_take(&pd->shared_mem_read_lock);
	list_init(&pd->threads);
	list_init(&pd->children);
	list_init(&pd->siblings);
	list_init(&pd->process_list_entry);
	list_init(&pd->mem_list);
	if (parent != NULL) {
		list_add_tail(&pd->siblings, &parent->children);
	}
	list_add_tail(&pd->process_list_entry, &process_list);

	if (parent != NULL)
        copy_file(pd, parent);
    pd->signalHandler = NULL;
	// 用户进程需要复制页表
	if (type == PROCESS_USER) {
		pd->cr3 = create_user_page(pd);  // 分配新的用户四级页表
	} else {
		pd->cr3 = read_cr3();
	}

	if (from == CREATE_PROCESS_FROM_FILE) {
	    // 从文件创建进程
	    size_t size = strlen((char *)main) + 1;
	    pd->path = kmalloc_s(size);
	    memcpy(pd->path, main, size);
        create_thread(pd, sys_real_exec);
    } else {
	    pd->path = NULL;
	    create_thread(pd, main);
    }
	return pid;
}

ProcessDescriptor *get_process(u16 pid) {
	ListEntry *c;
	// _si(pid);
	foreach(c, process_list) {
		ProcessDescriptor *pd = list_entry(c, ProcessDescriptor, process_list_entry);
		if (pd->pid == pid)
			return pd;
	}
	loge("no pid %d", pid);
	return NULL;
}

int do_create_process(ProcessType type, void *main) {
    int pid = create_process(thiscpu_var(current)->process, type, main, CREATE_PROCESS_FROM_EXISTS);
    return pid;
}

int do_create_process_from_file(const char *path) {
    pid_t pid = create_process(thiscpu_var(current)->process, PROCESS_USER, (void*)path, CREATE_PROCESS_FROM_FILE);
    return pid;
}

void destroy_process(ProcessDescriptor *process) {
    // 释放线性区
    kfree_s(strlen(process->path) + 1, process->path);
    u64 linear_size = (u64)process->linear_end - (u64)process->linear_start;
    int linear_cnt = (int)(linear_size / PAGESIZE);
    normal_pages_release(process->linear_start, process->cr3, linear_cnt);
    // 释放页表
    ListEntry *c;
    foreach (c, process->mem_list) {
        MemList *mem = list_entry(c, MemList, next);
        frame_release(mem->page);
        // 这里释放的主要是进程的页表，由于页表本身即将被释放，因此在页表中解除这些页的映射并无意义
        // normal_page_release(mem->addr, process->cr3);
        kfree_s(sizeof(MemList), mem);
    }
    // 关闭文件
    int i;
    forrange(i, 0, CFG_PROCESS_FDS) {
        if (process->fds[i] != NULL) {
            do_close(i);
        }
    }
    kfree_s(sizeof(ProcessDescriptor), process);

}

void destroy_thread(ThreadDescriptor *thread) {
    logd("destroy thread %d:%d", thread->process->pid, thread->tid);
    ProcessDescriptor *process = thread->process;
    void *st = thread->stack;
    int sz = thread->stack_length;
    while (sz--) {  // 释放用户页
    	st -= PAGESIZE;
    	pfn_t pfn = virt_to_pfn(st);
    	remove_from_mem_list(process, pfn);
        normal_page_release(st, thread->cr3);
    }
    // rsp0 rsp0值永远不变，每次进入中断都用一个空的内核栈
    if (process->shared_mem != thread->rsp0) {
		void *kst = thread->rsp0 - PAGESIZE;
		pfn_t kpg = virt_to_pfn(kst);
        remove_from_mem_list(process, kpg);
        kernel_page_release(kpg);
	}
	process->threads_cnt -= 1;
	list_remove(&thread->sibling);
	remove_thread_from_running(thread);
	if (process->threads_cnt == 0) {
		destroy_process(process);
	}
}

int do_quit_thread() {
    ThreadDescriptor *thread = thiscpu_var(current);
    destroy_thread(thread);
    logi("thread quit");
    sched_yield();
    while (1);
    return 0;
}

int do_getpid() {
    ThreadDescriptor *thread = thiscpu_var(current);
    return thread->process->pid;
}