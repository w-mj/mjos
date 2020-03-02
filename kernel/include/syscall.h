#include <asm.h>

#define SYS_CALL_0(n, name, ret) \
	static inline ret name() {\
		ASM("movq $"#n", %rdi")\
		ASM("syscall"); \
	}

#define SYS_CALL_1(n, name, ret, t1) \
	static inline ret name(t1 v1) { \
		ASM("movq $"#n", %rdx"); \
		ASM("movq %0, %%rdi" : : "rm" (v1));\
		ASM("syscall");\
	}


SYS_CALL_1(1, print_msg, void, char *)
