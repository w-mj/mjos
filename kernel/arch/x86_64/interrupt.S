#include <linkage.h>

EXTERN_FUNC(interrupt_stub)
GLOBAL(interrupt_stub_entry)
vec = 0
.rept 256
	pushq   %rdi
	movq    $vec,  %rdi
	call    interrupt_stub
	popq    %rdi
	vec = vec + 1
.endr
GLOBAL(interrupt_stub_entry_end)
