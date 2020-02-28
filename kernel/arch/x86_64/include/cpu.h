#pragma once
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
void prepare_switch(ThreadDescriber *prev, ThreadDescriber *next);
// 初始化线程栈，传入栈地址和线程的入口地址
void *init_thread_stack(void *sp, void *main);
void thread_switch();
