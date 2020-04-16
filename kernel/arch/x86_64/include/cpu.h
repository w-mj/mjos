#ifndef _OS_CPU_H
#define _OS_CPU_H
#include "asm.h"
#ifdef __cplusplus
extern "C" {
#endif

#include <process/process.h>
#include "base.h"
#include "types.h"
#include "arch.h"

extern int cpu_installed;
extern int cpu_activated;
extern u64 percpu_base;
extern u64 percpu_size;
extern void *isr_tbl[256];

typedef struct {
    u64 r15;
    u64 r14;
    u64 r13;
    u64 r12;
    u64 r11;
    u64 r10;
    u64 r9;
    u64 r8;
    u64 rbp;
    u64 rsi;
    u64 rdi;
    u64 rdx;
    u64 rcx;
    u64 rbx;
    u64 rax;
    u64 vec;
    u64 errcode;
    u64 rip;
    u64 cs;
    u64 rlfags;
    u64 rsp;
    u64 ss;
} InterruptContext;


typedef struct __SwitchContext{
    u64 r15;
    u64 r14;
    u64 r13;
    u64 r12;
    u64 r11;
    u64 r10;
    u64 r9;
    u64 r8;
    u64 rbp;
    u64 rsi;
    u64 rdi;
    u64 rdx;
    u64 rcx;
    u64 rbx;
    u64 rax;
    u64 rip;
    u64 cs;
    u64 rlfags;
    u64 rsp;
    u64 ss;
} SwitchContext;

int    cpu_count();
int    cpu_index();
void *calc_percpu_addr(u32 cpu, void * ptr);
void *calc_thiscpu_addr(void * ptr);

#define percpu_ptr(i, var)  ((TYPE(&var)) calc_percpu_addr(i, &var))
#define thiscpu_ptr(var)    ((TYPE(&var)) calc_thiscpu_addr(&var))
#define percpu_var(i, var)  (* percpu_ptr(i, var))
#define thiscpu_var(var)    (* thiscpu_ptr(var))

typedef struct __ThreadDescriptor ThreadDescriptor;
void prepare_switch(ThreadDescriptor *prev, ThreadDescriptor *next);
// 初始化线程栈，传入栈地址和线程的入口地址
void *init_thread_stack(void *sp, void *main, u32 cs, u32 ss);
static inline void *init_kernel_thread_stack(void *sp, void *main) {
    return init_thread_stack(sp, main, KERNEL_CODE_DEC, KERNEL_DATA_DEC);
}
void *init_user_thread_stack(ThreadDescriptor *thread, void *main);
void thread_switch();
void load_tid_next(ThreadDescriptor *);

void set_rsp0(u64);
void user_return(void *ip, void *rsp);

u64 user_call0(void *func);
u64 user_call1(void *p1, void *func);

void *get_ip();

static inline void* get_sp() {
    void* r;
    ASM("movq %%rsp, %0": "=r"(r));
    return r;
}
//void go_kernel();
//void go_user();

int do_kernel_return(void *rip, void *rsp);
#ifdef __cplusplus
}
#endif

#endif
