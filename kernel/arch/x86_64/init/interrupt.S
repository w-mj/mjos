#include <linkage.h>

#define save_regs               \
	pushq   %rax;               \
    pushq   %rbx;               \
    pushq   %rcx;               \
    pushq   %rdx;               \
    pushq   %rdi;               \
    pushq   %rsi;               \
    pushq   %rbp;               \
    pushq   %r8;                \
    pushq   %r9;                \
    pushq   %r10;               \
    pushq   %r11;               \
    pushq   %r12;               \
    pushq   %r13;               \
    pushq   %r14;               \
    pushq   %r15;

#define restore_regs            \
    popq    %r15;               \
    popq    %r14;               \
    popq    %r13;               \
    popq    %r12;               \
    popq    %r11;               \
    popq    %r10;               \
    popq    %r9;                \
    popq    %r8;                \
    popq    %rbp;               \
    popq    %rsi;               \
    popq    %rdi;               \
    popq    %rdx;               \
    popq    %rcx;               \
    popq    %rbx;               \
    popq    %rax;

EXTERN_FUNC(interrupt_stub)

// High |    SS    |
//      |   RSP    |
//      |  RFLAGS  |
//      |    CS    | <-- %rsp + 0x18
//      |   RIP    |
//      | ERR CODE |
// Low  |   RDI    | <-- %rsp
EXTERN_DATA(isr_tbl)
int_err_code:
	testl   $3, 0x18(%rsp)  // cs = rsp + 0x18 
    jz  1f   // 判断cs的特权级别，如果为用户态则交换gsbase和kgsbase
    swapgs 
1:

	xchgq   %rsi, 8(%rsp)  // 把rsi备份到栈里，同时取出error code
	movq    $isr_tbl, %rax
    movq    (%rax, %rdi, 8), %rax  // 查表调用相应的中断处理程序
    call    * %rax
	//  call   interrupt_stub
	popq    %rdi
	popq    %rsi

	testl  $3, 0x08(%rsp)  // 弹出rdi和errcode后，cs = rsp + 8
	jz 2f
	swapgs
2:
	iretq

#define with_errcode(vec) \
	pushq  %rdi           ;\
	movq   $vec, %rdi     ;\
	jmp int_err_code      ;\
	pushq $-1 // 不可能被执行到，占位

#define without_errcode(vec) \
	pushq $-1        ;\
	pushq %rdi               ;\
	movq  $vec, %rdi         ;\
	jmp int_err_code          


.balign 16
.section .init.text, "ax"
GLOBAL(interrupt_stub_entry)
vec = 0
.rept 256
	.if (((10 <= vec) && (vec <= 14)) || (vec == 17) || (vec == 1) || (vec == 30))
        with_errcode(vec)
    .else
        without_errcode(vec)
    .endif
	vec = vec + 1
	.balign 16
.endr
GLOBAL(interrupt_stub_entry_end)


EXTERN_DATA(tid_prev)
EXTERN_DATA(tid_next)

GLOBAL_FUNC(thread_switch)
GLOBAL_FUNC(load_tid_next)
// 进行线程切换，调用时栈顶为中断返回地址
// 函数调用时栈顶为进程切换回来后要执行的地址
// 此函数会保存当前线程上下文，再把栈构建成目标线程的中断上下文
// 通过iret执行目标线程
thread_switch:
	pushfq
	cli
	popq    %rax

    movq    %gs:(tid_prev), %rsi
    movq    %gs:(tid_next), %rdi

    movq    %cs, %r8                // r8  = cs
    popq    %r9                     // r9  = rip
    movq    %ss,  %r10              // r10 = ss
    movq    %rsp, %r11              // r11 = rsp
    pushq   %r10                    // ss
    pushq   %r11                    // rsp
    pushq   %rax                    // rflags
    pushq   %r8                     // cs
    pushq   %r9                     // rip
    pushq   $0                      // error code
    save_regs                       // 备份寄存器
    movq    %rsp, (%rsi)            // 备份当前的rsp

load_tid_next:
    movq    %rdi, %gs:(tid_prev)    // 新线程中的tid_prev保存当前线程的tid
    movq    0x00(%rdi), %rsp        // 切换上下文
	movq    0x10(%rdi), %rax        // rax = cr3
	movq    %rax, %cr3              // 换新线程页表
4:
    testl   $3, 0x88(%rsp)          // (15 + 2)* 8 = 0x88
    jz      3f
    swapgs
3:
    restore_regs                    // 恢复寄存器
	addq    $8, %rsp                // 跳过errcode
    iretq


// 初始化线程栈，传入栈地址和线程的入口地址
// void *init_thread_stack(void *sp, void *main)
GLOBAL_FUNC(init_thread_stack)
init_thread_stack:
	movq  %rsp, %r11  // 备份当前栈指针
	movq  %rdi, %rsp  // 设置栈指针
	movq  %ss,  %r10  // r10 = ss
	movq  %cs,  %r9   // r9  = cs

	pushq  %r10   // ss
	pushq  %rdi   // rsp
	pushfq        // rflags
	pushq  %r9    // cs
	pushq  %rsi   // ip = main
	pushq  $0     // errcode
	save_regs     // regs
	movq  %rsp, %rax  // 当前栈指针作为返回值

	movq  %r11, %rsp  // 恢复栈指针
	retq  
