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

// High |    SS    | +38
//      |   RSP    | +30
//      |  RFLAGS  | +28
//      |    CS    | +20
//      |   RIP    | +18
//      | ERR CODE | +10 (RSI)
//      |   RDI    | +8
//      |   RAX    | $rsp
EXTERN_DATA(isr_tbl)
int_err_code:
    pushq   %rax
    movq    $KERNEL_DATA_DEC, %rax
    movq    %rax, %ss

	xchgq   %rsi, 0x10(%rsp)  // 把rsi备份到栈里，同时取出error code

	testl   $3, 0x20(%rsp)  // cs = rsp + 0x20
    jz  1f   // 判断cs的特权级别，如果为用户态则交换gsbase和kgsbase
    swapgs
1:

	movq    $isr_tbl, %rax
    movq    (%rax, %rdi, 8), %rax  // 查表调用相应的中断处理程序
    call    * %rax

	testl  $3, 0x20(%rsp)   // %cs = %rsp + 0x20
	jz 2f
	swapgs
2:
    popq    %rax
	popq    %rdi
	popq    %rsi

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
	swapgs
    pushq %rcx
	pushq %r11

    // movq  %gs:(current), %rbp
    // movq  %rsp, (%rbp)      // 备份用户栈
	// movq  0x08(%rbp), %rsp  // 切换至内核栈
	movq  $syscall_tbl, %rax
	movq  (%rax, %r9, 8), %rax
	call  * %rax  // 系统调用例程的返回值会放在rax里

	//movq  $KERNEL_DATA_DEC, %rcx
	//movq  %rcx,  %ss
	//movq  $KERNEL_CODE_DEC, %rcx
	//movq  %rcx,  %cs
	// swapgs
    // popq  %rbx
    // movq  (%rbp), %rsp   // 恢复用户栈
	popfq
	// retq
	// popq %r11
	popq %rdi
	jmp  user_return

	sysret  // sysret 从MSR中找到用户态的cs和ss


#define USER_RETURN_TEMPLATE(reg_ip, reg_sp) \
    pushfq; \
    cli;    \
    popq    %rax; \
    pushq   $USER_DATA_DEC; \
    pushq   reg_sp; \
    pushq   %rax; \
    pushq   $USER_CODE_DEC; \
    pushq   reg_ip; \
    swapgs; \
    iretq; \

#define USER_RETURN_0 USER_RETURN_TEMPLATE(%rdi, %rsi)
#define USER_RETURN_1 USER_RETURN_TEMPLATE(%rsi, %rdx)

// void user_return(void *ip, void *rsp)
GLOBAL_FUNC(user_return)
user_return:
    USER_RETURN_0
/*
    pushfq
    cli                             // disable interrupt for now
    popq    %rax                    // rflags before cli
    pushq   $USER_DATA_DEC          // ss
    pushq   %rsi                    // rsp
    pushq   %rax                    // rflags
    pushq   $USER_CODE_DEC          // cs
    pushq   %rdi                    // rip
    swapgs
    iretq
*/

user_return1:
    USER_RETURN_1

#define USER_CALL_TEMPLATE(reg_sp) \
    pushq  %rsp; \
    pushq  $kernel_return ;\
    movq   %rsp, reg_sp
    /*
    popq   %rax; \
    addq   $-8, reg_sp; \
    movq   %rax, (reg_sp); \
    addq   $-8, reg_sp; \
    movq   %rsp, (reg_sp); \
    addq   $-8, reg_sp; \
    movq   $kernel_return1, (reg_sp)
    */
#define USER_CALL_0 USER_CALL_TEMPLATE(%rsi)
#define USER_CALL_1 USER_CALL_TEMPLATE(%rdx)

// u64 user_call0(void *func)
GLOBAL_FUNC(user_call0)
user_call0:
    USER_CALL_0
    call user_return
/*
    popq   %rax           // rax = rip
    addq   $-8, %rsi      // push %rip
    movq   %rax, (%rsi)
    addq   $-8, %rsi      // push %rsp
    movq   %rsp, (%rsi)
    jmp    user_return
*/


// u64 user_call1(void *param, void *func)
GLOBAL_FUNC(user_call1)
user_call1:
    USER_CALL_1
    call user_return1


kernel_return:
    movq  $0, %r9
    popq  %rsi     // %rsi = %rsp
    popq  %rdi     // %rdi = %rip
    syscall

// int do_kernel_return(void *rip, void *rsp);
GLOBAL_FUNC(do_kernel_return)
do_kernel_return:
    movq %rsi, %rsp
    pushq %rdi
    ret


EXTERN_DATA(tid_prev)
EXTERN_DATA(tid_next)
EXTERN_FUNC(before_schedule_in)
GLOBAL_FUNC(thread_switch)
GLOBAL_FUNC(load_tid_next)
// 进行线程切换，调用时栈顶为中断返回地址
// 函数调用时栈顶为进程切换回来后要执行的地址
// 此函数会保存当前线程上下文，再把栈构建成目标线程的中断上下文
// 通过iret执行目标线程
// High |    SS    | +58
//      |   RSP    | +50
//      |  RFLAGS  | +48
//      |    CS    | +40
//      |   RIP    | +38
//      | ERR CODE | +30
//      |   RDI    | +28
//      |   RAX    | +20
//      |   RBX    | +18
//      |   CR3    | +10
//      |CALL *RAX | +8
// LOW  |CAL switch| <-- %rsp
thread_switch:
	pushfq
	cli
	popq    %rax

    movq    %gs:(tid_prev), %rsi
    movq    %gs:(tid_next), %rdi

    movq    %cs, %r8            // r8  = cs
    popq    %r9                 // r9  = rip
    movq    %ss, %r10           // r10 = ss
    movq    %rsp, %r11          // r11 = rsp

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

/*
    movq    $0,   %rdi
    testl   $3, 0x88(%rsp)          // (15 + 3 - 1)* 8 = 0x88
    jz      1f
    movq    0x98(%rsp), %rdi        // rdi = rsp
1:
*/

    call    before_schedule_in
    testl   $3, 0x88(%rsp)          // (15 + 3 - 1)* 8 = 0x88
    jz      3f
    swapgs
    // movw    $USER_DATA_DEC, %ax
    // movw    %ax, %ds
    // movw    %ax, %es
    // movw    %ax, %fs
    // movw    %ax, %gs
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
	pushq  %rdx   // cs
	pushq  %rsi   // ip = main
	pushq  $0     // errcode
	save_regs     // regs
	movq  %rsp, %rax  // 当前栈指针作为返回值

	movq  %r11, %rsp  // 恢复栈指针
	retq

quit_thread_stub:
    movq  $6, %r9
    syscall

// void *init_user_thread_stack(ThreadDescriptor *thread, void *main);
GLOBAL_FUNC(init_user_thread_stack)
init_user_thread_stack:
    movq  %rsp, %r9
    movq  %cr3, %r10
    movq  (%rdi), %rsp
    movq  0x10(%rdi), %rax
    movq  %rax, %cr3
    pushq $quit_thread_stub   // main返回时执行退出线程
    movq  %rsp, (%rdi)
    pushq $USER_DATA_DEC
    pushq (%rdi)
    pushfq
    pushq $USER_CODE_DEC
    pushq %rsi
    pushq $0
    save_regs
    movq  %rsp, %rax
    movq  %r10, %cr3
    movq  %r9,  %rsp
    ret

GLOBAL_FUNC(get_ip)
get_ip:
    movq 0(%rsp), %rax
    ret


/*
GLOBAL_FUNC(go_kernel)
go_kernel:
    movl   %cs, %eax
    test   $3, %eax
    jz     1f
    swapgs
1:
    ret

GLOBAL_FUNC(go_user)
go_user:
    movl  %cs, %eax
    test  $3,  %eax
    jnz   1f
    swapgs
1:
    ret
*/
