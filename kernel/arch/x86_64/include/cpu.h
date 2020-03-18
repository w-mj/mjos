#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <base.h>

extern int cpu_installed;
extern int cpu_activated;
extern u64 percpu_base;
extern u64 percpu_size;
extern void *isr_tbl[256];

int    cpu_count();
int    cpu_index();
void *calc_percpu_addr(u32 cpu, void * ptr);
void *calc_thiscpu_addr(void * ptr);

#define percpu_ptr(i, var)  ((TYPE(&var)) calc_percpu_addr(i, &var))
#define thiscpu_ptr(var)    ((TYPE(&var)) calc_thiscpu_addr(&var))
#define percpu_var(i, var)  (* percpu_ptr(i, var))
#define thiscpu_var(var)    (* thiscpu_ptr(var))

#include <process/process.h>
#include "arch.h"

void prepare_switch(ThreadDescriptor *prev, ThreadDescriptor *next);
// 初始化线程栈，传入栈地址和线程的入口地址
void *init_thread_stack(void *sp, void *main, u32 cs, u32 ss);
static inline void *init_kernel_thread_stack(void *sp, void *main) {
    return init_thread_stack(sp, main, KERNEL_CODE_DEC, KERNEL_DATA_DEC);
}
void *init_user_thread_stack(ThreadDescriptor *thread, void *main);
void thread_switch();

void set_rsp0(u64);
#ifdef __cplusplus
}
#endif
