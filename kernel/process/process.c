#include <string.h>
#include <list.h>
#include <base.h>
#include <delog.h>
#include <asm.h>
#include <cpu.h>
#include <memory/page.h>
#include <memory/kmalloc.h>
#include <process/schedule.h>
#include <process/process.h>

static u8 *pid_bitmap = NULL;


void process_init() {
	logd("init process");
	pid_bitmap = pfn_to_virt(kernel_page_alloc(PG_KERNEL));
	memset(pid_bitmap, 0, PAGESIZE);
}

static u16 find_usable_pid() {
	u16 pid = 0;
	while(pid < 32767) {
		if (chk_bitmap(pid_bitmap, pid)) {
			set_bitmap(pid_bitmap, pid);
			return pid;
		}
		pid++;
	}
	return (u16)-1;
}

ThreadDescriber *create_thread(ProcessDescriber *process, void *main) {
	ThreadDescriber *thread = kmalloc_s(sizeof(ThreadDescriber));
	current = thread;
	void *stack = normal_page_alloc(NULL);  
	stack = stack + PAGESIZE;  // stack 指向栈的尾部，向下增长
	thread->rsp = init_thread_stack(stack, main);
	thread->rsp0 = thread->rsp;
	thread->cr3 = process->cr3;
	thread->tid = process->threads_cnt;
	process->threads_cnt++;
	list_init(&thread->next);
	add_thread_to_running(thread);
	return thread;
}

void create_process(ProcessDescriber *parent, ProcessType type, void *main) {
	u16 pid = find_usable_pid();
	if (pid == (u16)-1) {
		loge("no usable pid.");
		die();
	}

	ProcessDescriber *pd = kmalloc_s(sizeof(ProcessDescriber));
	pd->pid = pid;
	pd->type = type;
	pd->parent = parent;
	pd->threads_cnt = 0;
	list_init(&pd->threads);
	list_init(&pd->children);
	list_init(&pd->sublings);
	if (parent != NULL) {
		list_add_tail(&pd->sublings, &parent->children);
	}

	// 用户进程需要复制页表
	if (type == PROCESS_USER) {
		// TODO: 创建用户进程页表
	} else {
		pd->cr3 = read_cr3();
	}
	create_thread(pd, main);
}

