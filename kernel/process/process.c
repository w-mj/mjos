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

ThreadDescriber *create_thread(ProcessDescriber *process, void *main) {
	logd("create thread %d for pid %d", process->threads_cnt, process->pid);
	ThreadDescriber *thread = kmalloc_s(sizeof(ThreadDescriber));
	current = thread;
	void *stack = normal_page_alloc(NULL);  
	stack = stack + PAGESIZE;  // stack 指向栈的尾部，向下增长
	thread->rsp = init_thread_stack(stack, main);
	thread->rsp0 = thread->rsp;
	thread->cr3 = process->cr3;
	thread->tid = process->threads_cnt;
	thread->process = process;
	process->threads_cnt++;
	list_init(&thread->next);
	list_add_tail(&thread->next, &process->threads);
	add_thread_to_running(thread);
	return thread;
}

u16 create_process(ProcessDescriber *parent, ProcessType type, void *main) {
	pid_t pid = find_usable_pid();
	if (pid == (u16)-1) {
		loge("no usable pid.");
		die();
	}
	logd("create process %d", pid);

	ProcessDescriber *pd = kmalloc_s(sizeof(ProcessDescriber));
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
		assert(0);
		// TODO: 创建用户进程页表
	} else {
		pd->cr3 = read_cr3();
	}
	create_thread(pd, main);
	return pid;
}

ProcessDescriber *get_process(u16 pid) {
	ListEntry *c;
	_si(pid);
	foreach(c, process_list) {
		ProcessDescriber *pd = list_entry(c, ProcessDescriber, process_list_entry);
		if (pd->pid == pid)
			return pd;
	}
	loge("no pid %d", pid);
	return NULL;
}
