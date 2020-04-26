
#ifndef OS_PROCESS_H
#define OS_PROCESS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "list.h"
#include "biosreg.h"
#include "spin.h"

#include "config.h"
#include "process/signal.h"

typedef enum {
	THREAD_RUNNING,
	THREAD_BLOCKED,
	THREAD_STOPPED,
} ThreadState;

typedef enum {
	PROCESS_USER,
	PROCESS_KERNEL
} ProcessType;

typedef enum {
    CREATE_PROCESS_FROM_EXISTS,
    CREATE_PROCESS_FROM_FILE
} CreateProcessFrom;

struct __ProcessDescriptor;

typedef struct __MemList {
	pfn_t page;
	void *addr;
	ListEntry next;
} MemList;

enum ThreadWaitType {
    ThreadRunning,
    ThreadWaitPid,
    ThreadWaitDMA,
    ThreadWaitFile,
    ThreadWaitMutex
};

typedef struct __ThreadDescriptor{
	void *rsp;
	void *rsp0;
	// 同一个进程的所有线程的cr3都相同
	// 这里冗余的cr3为了在汇编语言里寻址方便，以后可以优化掉
	u64 cr3;
    void *stack;  // 栈顶地址
	// **以上所有成员位置不可改变**


	ThreadState state;
	struct __ProcessDescriptor *process;
	ListEntry next;   // 用于调度队列
	ListEntry sibling;  // 兄弟线程
	int tid;
	int stack_length;  // 栈长度(单位：页）

	enum ThreadWaitType waitType;
	u64 waitValue;
} ThreadDescriptor;


typedef struct __ProcessDescriptor {
	pid_t pid;   // 0~32767
	ProcessType type;
	struct __ProcessDescriptor *parent;
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
	void *cwd;  // 当前的目录，类型为指向VFS::DEntry的指针
    SignalHandler signalHandler;   // 信号处理函数，必须在内核空间
    void *linear_start; // 线性区起始地址，通常为0x400000
    void *linear_end;   // 线性区终止地址
    void *heap_start;   // 堆起始地址，堆起始地址就是代码区的结束位置
    void *heap_end;     // 堆终止地址，堆终止地址总小于等于线性区终止地址
    char *path;   // 进程路径
    ListEntry signal_ist;   // 信号队列

} ProcessDescriptor;

extern ThreadDescriptor *current;


void process_init();
pid_t create_process(ProcessDescriptor *parent, ProcessType type, void *main, CreateProcessFrom from);
ThreadDescriptor *create_thread(ProcessDescriptor *process, void *main);
ProcessDescriptor *get_process(pid_t pid);

void parse_elf64(void *);
extern void *user_processes[];

int do_quit_thread();
void add_to_mem_list(ProcessDescriptor *process, pfn_t pfn, void *addr);
void remove_from_mem_list(ProcessDescriptor *process, pfn_t pfn);

int do_getpid();
int do_create_process_from_file(const char *);
int do_waitpid(pid_t);

#ifdef __cplusplus
};
#endif

#endif

