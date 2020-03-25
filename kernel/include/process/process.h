#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <biosreg.h>
#include <spin.h>

#include <config.h>

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

typedef struct __MemList {
	pfn_t page;
	void *addr;
	ListEntry next;
} MemList;

typedef struct {
	void *rsp;
	void *rsp0;
	// 同一个进程的所有线程的cr3都相同
	// 这里冗余的cr3为了在汇编语言里寻址方便，以后可以优化掉
	u64 cr3;
	ThreadState state;
	struct __ProcessDescriber *process;
	ListEntry next;   // 用于调度队列
	ListEntry sibling;  // 兄弟线程
	int tid;
	void *stack;  // 栈顶地址
	int stack_length;  // 栈长度(单位：页）
} ThreadDescriptor;


typedef struct __ProcessDescriber {
	pid_t pid;   // 0~32767
	ProcessType type;
	struct __ProcessDescriber *parent;
	ListEntry threads;
	ListEntry children;  // list head
	ListEntry siblings;  // list entry
	u64 cr3;
	int threads_cnt;
	ListEntry process_list_entry;
	char *shared_mem;
	Spin shared_mem_read_lock;
	Spin shared_mem_write_lock;
	ListEntry mem_list;
	void *fds[CFG_PROCESS_FDS];  // 打开的文件列表，每一个元素是一个指向VFS::File的指针
} ProcessDescriptor;

extern ThreadDescriptor *current;


void process_init();
pid_t create_process(ProcessDescriptor *parent, ProcessType type, void *main);
ThreadDescriptor *create_thread(ProcessDescriptor *process, void *main);
ProcessDescriptor *get_process(pid_t pid);

void parse_elf64(void *);
extern void *user_processes[];

int do_quit_thread();
void add_to_mem_list(ProcessDescriptor *process, pfn_t pfn, void *addr);
void remove_from_mem_list(ProcessDescriptor *process, pfn_t pfn);

#ifdef __cplusplus
}
#endif
