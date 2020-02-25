#pragma once
#include <types.h>
#include <list.h>
#include <biosreg.h>

typedef enum {
	THEREAD_RUNNING,
	THEREAD_INTERRUPTIBLE,
	THEREAD_UNINTERRUPTIBLE,
	THEREAD_STOPPED,
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
} __PACKED ThreadDescriber;


typedef struct __ProcessDescriber {
	u16 pid;   // 0~32767
	ProcessType type;
	struct __ProcessDescriber *parent;
	ListEntry threads;
	ListEntry children;  // list head
	ListEntry sublings;  // list entry
	u64 cr3;
} ProcessDescriber;

extern ThreadDescriber *current;
