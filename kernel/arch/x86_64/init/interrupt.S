#include <linkage.h>
#include <arch.h>

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
	.if (((10 <= vec) && (vec <= 14)) || (vec == 17) || (vec == 8) || (vec == 30))
        with_errcode(vec)
    .else
        without_errcode(vec)
    .endif
	vec = vec + 1
	.balign 16
.endr
GLOBAL(interrupt_stub_entry_end)

// 由Syscall 进入的地址
EXTERN_DATA(syscall_tbl)
EXTERN_DATA(kernel_pml4)  // 内核页表
EXTERN_DATA(boot_stack_top)
GLOBAL_FUNC(syscall_stub)
syscall_stub:
	// rcx = rip
	// r11 = rflags
	// r9  = 调用号
	// rdi = 参数一
	// rsi = 参数二
	// rdx = 参数三
	// rcx = 参数四
	// r8  = 参数五
	// swapgs
	pushq %rcx
	pushq %r11
	pushq %rbp
	movq  %cr3, %r10   // r10 = cr3
	pushq %r10

	movq  %rsp, %rbp   // 备份用户栈
	movq  $boot_stack_top, %rsp  // 切换至内核栈
	movq  $kernel_pml4, %rax
	movq  (%rax), %r10 // 切换至内核页表
	movq  %r10 , %cr3
	movq  $syscall_tbl, %rax
	movq  (%rax, %r9, 8), %rax
	call  * %rax  // 系统调用例程的返回值会放在rax里

	//movq  $KERNEL_DATA_DEC, %rcx
	//movq  %rcx,  %ss
	//movq  $KERNEL_CODE_DEC, %rcx
	//movq  %rcx,  %cs
	// swapgs
	movq  %rbp, %rsp   // 恢复用户栈
	popq  %r10
	movq  %r10, %cr3
	popq  %rbp
	popfq
	retq
	// sysret  // sysret 从MSR中找到用户态的cs和ss


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
// void *init_thread_stack(void *sp, void *main, u32 cs, u32 ss)
GLOBAL_FUNC(init_thread_stack)
init_thread_stack:
	movq  %rsp, %r11  // 备份当前栈指针
	movq  %rdi, %rsp  // 设置栈指针
	// movq  %ss,  %r10  // r10 = ss
	// movq  %cs,  %r9   // r9  = cs

	pushq  %rcx   // ss
	pushq  %rdi   // rsp
	pushfq        // rflags
	pushq  %rdx    // cs
	pushq  %rsi   // ip = main
	pushq  $0     // errcode
	save_regs     // regs
	movq  %rsp, %rax  // 当前栈指针作为返回值

	movq  %r11, %rsp  // 恢复栈指针
	retq  
