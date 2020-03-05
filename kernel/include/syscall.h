#include <asm.h>
#include <process/process.h>

#define PREPARE(n) ASM("movq $"#n", %r9")
#define PARAM1(v)  ASM("movq %0, %%rdi" :: "rm"(v))
#define PARAM2(v)  ASM("movq %0, %%rsi" :: "rm"(v))
#define PARAM3(v)  ASM("movq %0, %%rdx" :: "rm"(v))
#define PARAM4(v)  ASM("movq %0, %%rcx" :: "rm"(v))
#define PARAM5(v)  ASM("movq %0, %%r8"  :: "rm"(v))
#define CALL       ASM("syscall")
#define RET        ASM("leaveq; retq"); return 0

#define SYS_CALL_0(n, name, ret) \
	static inline ret name() {\
		PREPARE(n); \
		CALL;	    \
		RET;        \
	}

#define SYS_CALL_1(n, name, t1) \
	static inline int name(t1 v1) { \
		PREPARE(n); \
		PARAM1(v1); \
		CALL;       \
		RET;        \
	} 

#define SYS_CALL_2(n, name, t1, t2) \
	static inline int name(t1 v1, t2 v2) {\
		PREPARE(n); \
		PARAM1(v1); \
		PARAM2(v2); \
		CALL;       \
		RET;        \
	}

#define SYS_CALL_3(n, name, t1, t2, t3) \
	static inline int name(t1 v1, t2 v2, t3 v3) {\
		PREPARE(n); \
		PARAM1(v1); \
		PARAM2(v2); \
		PARAM3(v3); \
		CALL;       \
		RET;        \
	}

#define SYS_CALL_4(n, name, t1, t2, t3, t4) \
	static inline int name(t1 v1, t2 v2, t3 v3, t4 v4) {\
		PREPARE(n); \
		PARAM1(v1); \
		PARAM2(v2); \
		PARAM3(v3); \
		PARAM3(v4); \
		CALL;       \
		RET;        \
	}


#define SYS_CALL_5(n, name, t1, t2, t3, t4, t5) \
	static inline int name(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5) {\
		PREPARE(n); \
		PARAM1(v1); \
		PARAM2(v2); \
		PARAM3(v3); \
		PARAM4(v4); \
		PARAM5(v5); \
		CALL;       \
		RET;        \
	}


SYS_CALL_1(1, sys_print_msg, char *)
SYS_CALL_2(2, sys_create_process, ProcessType, void *);