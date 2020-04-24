#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "process/process.h"

void add_thread_to_running(ThreadDescriptor *);
void remove_thread_from_running(ThreadDescriptor *);

void scheduler_init();
// schedule 应该在中断环境中被调用，此函数会设置好tid_next和tid_prev，
// 再在中断中紧接着调用thread_switch
bool schedule();

#define sched_yield() schedule()
void thread_wait(enum ThreadWaitType type, u64 value);
void thread_resume(enum ThreadWaitType type, u64 value);
#ifdef __cplusplus
}
#endif
