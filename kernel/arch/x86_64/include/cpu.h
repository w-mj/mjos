#pragma once
#include <base.h>

int cpu_installed;
int cpu_activated;
u64 percpu_base;
u64 percpu_size;

int    cpu_count();
int    cpu_index();
void *calc_percpu_addr(u32 cpu, void * ptr);
void *calc_thiscpu_addr(void * ptr);

#define percpu_ptr(i, var)  ((TYPE(&var)) calc_percpu_addr(i, &var))
#define thiscpu_ptr(var)    ((TYPE(&var)) calc_thiscpu_addr(&var))
#define percpu_var(i, var)  (* percpu_ptr(i, var))
#define thiscpu_var(var)    (* thiscpu_ptr(var))
