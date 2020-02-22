#pragma once
#include <types.h>
#include <list.h>
#include <biosreg.h>

typedef enum {
	TASK_RUNNING,
	TASK_INTERRUPTIBLE,
	TASK_UNINTERRUPTIBLE,
	TASK_STOPPED,
} TaskState;

typedef struct {
	void *rsp;
	void *rsp0;
	int tid;
} ThreadDescriber;


typedef struct __ProcessDescriber {
	u16 pid;   // 0~32767
	TaskState state;
	struct __ProcessDescriber *parent;
	ListEntry threads;
	ListEntry children;
	ListEntry sublings;
	u64 cr3;
} ProcessDescriber;
