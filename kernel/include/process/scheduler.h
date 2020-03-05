#pragma once
#include <process/process.h>

void add_thread_to_running(ThreadDescriptor *);

void scheduler_init();
// schedule 应该在中断环境中被调用，此函数会设置好tid_next和tid_prev，
// 再在中断中紧接着调用thread_switch
bool schedule();
