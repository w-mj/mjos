#pragma once

#ifndef __ASSEMBLER__
void idt_init();
void gdt_init();
void tss_init();
void cpu_init();
void per_cpu_init();

// 初始化线程栈，传入栈地址和线程的入口地址
void *init_thread_stack(void *sp, void *main);
#endif

