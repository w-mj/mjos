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

static u8 *pid_bitmap = NULL;
static ListEntry process_list;

void process_init() {
	logd("init process");
	pid_bitmap = pfn_to_virt(kernel_page_alloc(PG_KERNEL));
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

ThreadDescriptor *create_thread(ProcessDescriptor *process, void *main) {
	logd("create thread %d for pid %d", process->threads_cnt, process->pid);
	ThreadDescriptor *thread = kmalloc_s(sizeof(ThreadDescriptor));
	_sL(process->cr3);
	void *stack = normal_page_alloc(NULL, process->cr3);
	void *sp0 = pfn_to_virt(kernel_page_alloc(PG_KERNEL));
	u64* page = phys_to_virt(process->cr3);
	_sL(page[0]);
	_sL(page[511]);
	stack = stack + PAGESIZE;  // stack 指向栈的尾部，向下增长
	if (process->type == PROCESS_KERNEL) {
		thread->rsp = init_thread_stack(stack, main, KERNEL_CODE_DEC, KERNEL_DATA_DEC);
	} else {
		u64 kcr3 = read_cr3();
		u64 ucr3 = process->cr3;
		u64 ip = 0xffffffff81016a5d;
		_sL(page_translate(kcr3, ip));
		_sL(page_translate(ucr3, ip));
		write_cr3(ucr3);  // 切换成用户页表
		thread->rsp = init_thread_stack(stack, main, USER_CODE_DEC, USER_DATA_DEC);
		write_cr3(kcr3);
	}
	thread->rsp0 = sp0;
	thread->cr3 = process->cr3;
	thread->tid = process->threads_cnt;
	thread->process = process;
	process->threads_cnt++;
	list_init(&thread->next);
	list_add_tail(&thread->next, &process->threads);
	add_thread_to_running(thread);
	return thread;
}

pid_t create_process(ProcessDescriptor *parent, ProcessType type, void *main) {
	pid_t pid = find_usable_pid();
	if (pid == (u16)-1) {
		loge("no usable pid.");
		die();
	}
	logd("create process %d", pid);

	ProcessDescriptor *pd = kmalloc_s(sizeof(ProcessDescriptor));
	pd->pid = pid;
	pd->type = type;
	pd->parent = parent;
	pd->threads_cnt = 0;
	list_init(&pd->threads);
	list_init(&pd->children);
	list_init(&pd->sublings);
	list_init(&pd->process_list_entry);
	if (parent != NULL) {
		list_add_tail(&pd->sublings, &parent->children);
	}
	list_add_tail(&pd->process_list_entry, &process_list);

	// 用户进程需要复制页表
	if (type == PROCESS_USER) {
		pd->cr3 = create_user_page();  // 分配新的用户四级页表
	} else {
		pd->cr3 = read_cr3();
	}
	create_thread(pd, main);
	return pid;
}

ProcessDescriptor *get_process(u16 pid) {
	ListEntry *c;
	_si(pid);
	foreach(c, process_list) {
		ProcessDescriptor *pd = list_entry(c, ProcessDescriptor, process_list_entry);
		if (pd->pid == pid)
			return pd;
	}
	loge("no pid %d", pid);
	return NULL;
}

pid_t do_sys_create_process(ProcessType type, void *main) {
    return create_process(thiscpu_var(current)->process, type, main);
}