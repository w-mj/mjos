#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <asm.h>
#include <process/process.h>
#include <delog.h>
#include <stddef.h>
#include <process/signal.h>
#include <stdio.h>

#define PREPARE(n) ASM("movq $"#n", %r9"); \
    do {\
        int cs;\
        ASM("mov %%cs, %0": "=r"(cs)); \
        if ((cs & 0x3) == 0) { \
            _ss("sys call shall not be called in kernel mode."); \
            while (1); \
            return -1; \
        } \
    } while(0)
#define PARAM1(v)  ASM("movq %0, %%rdi" :: "rm"(v))
#define PARAM2(v)  ASM("movq %0, %%rsi" :: "rm"(v))
#define PARAM3(v)  ASM("movq %0, %%rdx" :: "rm"(v))
#define PARAM4(v)  ASM("movq %0, %%rcx" :: "rm"(v))
#define PARAM5(v)  ASM("movq %0, %%r8"  :: "rm"(v))
#define CALL       ASM("syscall")
#define RET        ASM("leaveq; retq"); return 0

#define SYS_CALL_0(n, name) \
    int do_##name(); \
	static inline int sys_##name() {\
		PREPARE(n); \
		CALL;	    \
		RET;        \
	}

#define SYS_CALL_1(n, name, t1) \
    int do_##name(t1);  \
	static inline int sys_##name(t1 v1) { \
		PREPARE(n); \
		PARAM1(v1); \
		CALL;       \
		RET;        \
	} 

#define SYS_CALL_2(n, name, t1, t2) \
    int do_##name(t1, t2);  \
	static inline int sys_##name(t1 v1, t2 v2) {\
		PREPARE(n); \
		PARAM1(v1); \
		PARAM2(v2); \
		CALL;       \
		RET;        \
	}

#define SYS_CALL_3(n, name, t1, t2, t3) \
    int do_##name(t1, t2, t3);  \
	static inline int sys_##name(t1 v1, t2 v2, t3 v3) {\
		PREPARE(n); \
		PARAM1(v1); \
		PARAM2(v2); \
		PARAM3(v3); \
		CALL;       \
		RET;        \
	}

#define SYS_CALL_4(n, name, t1, t2, t3, t4) \
    int do_##name(t1, t2, t3, t4);  \
	static inline int sys_##name(t1 v1, t2 v2, t3 v3, t4 v4) {\
		PREPARE(n); \
		PARAM1(v1); \
		PARAM2(v2); \
		PARAM3(v3); \
		PARAM3(v4); \
		CALL;       \
		RET;        \
	}


#define SYS_CALL_5(n, name, t1, t2, t3, t4, t5) \
    int do_##name(t1, t2, t3, t4, t5);  \
	static inline int sys_##name(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5) {\
		PREPARE(n); \
		PARAM1(v1); \
		PARAM2(v2); \
		PARAM3(v3); \
		PARAM4(v4); \
		PARAM5(v5); \
		CALL;       \
		RET;        \
	}


SYS_CALL_1(1, print_msg, const char *)
SYS_CALL_2(2, create_process, ProcessType, void *)
SYS_CALL_3(3, send_message, pid_t, const char *, int)
SYS_CALL_1(4, read_message, char *)
SYS_CALL_1(5, func_register, int)
SYS_CALL_0(6, quit_thread)
SYS_CALL_1(7, open, const char *)
SYS_CALL_3(8, read, int, char *, size_t)
SYS_CALL_3(9, write, int, const char *, size_t)
SYS_CALL_1(10, close, int)
SYS_CALL_0(11, getpid)
SYS_CALL_3(12, signal, SignalType, uint64_t, pid_t)
SYS_CALL_1(13, create_process_from_file, const char *)
SYS_CALL_2(14, get_attr, int, struct FILE*)
SYS_CALL_0(15, real_exec)
SYS_CALL_1(16, exec, const char *)

enum {
    SYS_FUNC_PRINTMSG = 0,
    SYS_FUNC_END
};

#ifdef __cplusplus
}
#endif
