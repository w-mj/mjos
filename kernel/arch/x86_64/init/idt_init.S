#include <linkage.h>

EXTERN_FUNC(interrupt_stub)
.balign 16
GLOBAL(interrupt_stub_entry)
vec = 0
.rept 256
	pushq   %rdi
	movq    $vec,  %rdi
	call    interrupt_stub
	popq    %rdi
	iretq
	vec = vec + 1
	.balign 16
.endr
GLOBAL(interrupt_stub_entry_end)
