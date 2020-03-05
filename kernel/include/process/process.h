#pragma once
#include <types.h>
#include <list.h>
#include <biosreg.h>

typedef enum {
	THREAD_RUNNING,
	THREAD_INTERRUPTIBLE,
	THREAD_UNINTERRUPTIBLE,
	THREAD_STOPPED,
} ThreadState;

typedef enum {
	PROCESS_USER,
	PROCESS_KERNEL
} ProcessType;

struct __ProcessDescriber;

typedef struct {
	void *rsp;
	void *rsp0;
	// 同一个进程的所有线程的cr3都相同
	// 这里冗余的cr3为了在汇编语言里寻址方便，以后可以优化掉
	u64 cr3;
	ThreadState state;
	struct __ProcessDescriber *process;
	ListEntry next;   // 用于调度队列
	int tid;
} ThreadDescriber;


typedef struct __ProcessDescriber {
	pid_t pid;   // 0~32767
	ProcessType type;
	struct __ProcessDescriber *parent;
	ListEntry threads;
	ListEntry children;  // list head
	ListEntry sublings;  // list entry
	u64 cr3;
	int threads_cnt;
	ListEntry process_list_entry;
} ProcessDescriber;

extern ThreadDescriber *current;


void process_init();
pid_t create_process(ProcessDescriber *parent, ProcessType type, void *main);
ThreadDescriber *create_thread(ProcessDescriber *process, void *main);
ProcessDescriber *get_process(pid_t pid);

pid_t do_sys_create_process(ProcessType, void *);

void parse_elf64(void *);
