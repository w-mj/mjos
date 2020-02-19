#include <linkage.h>

EXTERN_FUNC(interrupt_stub)


int_err_code:
	xchgq   %rsi, 8(%rsp)  // 把rsi备份到栈里，同时取出error code
	call   interrupt_stub
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
	.if (((10 <= vec) && (vec <= 14)) || (vec == 17) || (vec == 1) || (vec == 30))
        with_errcode(vec)
    .else
        without_errcode(vec)
    .endif
	vec = vec + 1
	.balign 16
.endr
GLOBAL(interrupt_stub_entry_end)
