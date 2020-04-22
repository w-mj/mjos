#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "process/process.h"
#include "stdint.h"
#include "sys/types.h"
#include "process/signal.h"
#include "asm.h"
#include "stdio.h"

#include "sys/kstat.h"

//struct stat;
//struct tms;
//struct timeval;
//struct timezone;
#ifdef KERNEL
#define PREPARE(n) ASM("movq $"#n", %r9"); \
    do {\
        int cs;\
        ASM("mov %%cs, %0": "=r"(cs)); \
        if ((cs & 0x3) == 0) { \
            while (1); \
        } \
    } while(0)
#else
#define PREPARE(n) ASM("movq $"#n", %r9")
#endif
#define PARAM1(v)  ASM("movq %0, %%rdi" :: "rm"((uint64_t)v))
#define PARAM2(v)  ASM("movq %0, %%rsi" :: "rm"((uint64_t)v))
#define PARAM3(v)  ASM("movq %0, %%rdx" :: "rm"((uint64_t)v))
#define PARAM4(v)  ASM("movq %0, %%rcx" :: "rm"((uint64_t)v))
#define PARAM5(v)  ASM("movq %0, %%r8"  :: "rm"((uint64_t)v))
#define CALL       ASM("syscall; nop;")
// #define RET        ASM("leaveq; retq"); return 0
#define RET(type)      \
    do {\
        type a;\
        switch (sizeof(type)) {\
            case 4: \
                ASM("movl %%eax, %0": "=rm"(a)); \
            break;\
            case 8: \
                ASM("movq %%rax, %0": "=rm"(a)); \
                break;\
        }\
        return a;\
    } while (0)


#define SYS_CALL_0(n, ret, name) \
    ret do_##name(); \
	static inline ret sys_##name() {\
		PREPARE(n); \
		CALL;	    \
		RET(ret);        \
	}

#define SYS_CALL_1(n, ret, name, t1) \
    ret do_##name(t1);  \
	static inline ret sys_##name(t1 v1) { \
		PREPARE(n); \
		PARAM1(v1); \
		CALL;       \
		RET(ret);        \
	}

#define SYS_CALL_2(n, ret, name, t1, t2) \
    ret do_##name(t1, t2);  \
	static inline ret sys_##name(t1 v1, t2 v2) {\
		PREPARE(n); \
		PARAM1(v1); \
		PARAM2(v2); \
		CALL;       \
		RET(ret);        \
	}

#define SYS_CALL_3(n, ret, name, t1, t2, t3) \
    ret do_##name(t1, t2, t3);  \
	static inline ret sys_##name(t1 v1, t2 v2, t3 v3) {\
		PREPARE(n); \
		PARAM1(v1); \
		PARAM2(v2); \
		PARAM3(v3); \
		CALL;       \
		RET(ret);        \
	}

#define SYS_CALL_4(n, ret, name, t1, t2, t3, t4) \
    ret do_##name(t1, t2, t3, t4);  \
	static inline ret sys_##name(t1 v1, t2 v2, t3 v3, t4 v4) {\
		PREPARE(n); \
		PARAM1(v1); \
		PARAM2(v2); \
		PARAM3(v3); \
		PARAM3(v4); \
		CALL;       \
		RET(ret);        \
	}


#define SYS_CALL_5(n, ret, name, t1, t2, t3, t4, t5) \
    ret do_##name(t1, t2, t3, t4, t5);  \
	static inline ret sys_##name(t1 v1, t2 v2, t3 v3, t4 v4, t5 v5) {\
		PREPARE(n); \
		PARAM1(v1); \
		PARAM2(v2); \
		PARAM3(v3); \
		PARAM4(v4); \
		PARAM5(v5); \
		CALL;       \
		RET(ret);        \
	}


SYS_CALL_1(1 ,int, print_msg, const char *)
SYS_CALL_2(2 ,int, create_process, ProcessType, void *)
SYS_CALL_3(3 ,int, send_message, pid_t, const char *, int)
SYS_CALL_1(4 ,int, read_message, char *)
SYS_CALL_1(5 ,int, func_register, int)
SYS_CALL_0(6 ,int, quit_thread)
SYS_CALL_1(7 ,int, open, const char *)
SYS_CALL_3(8 ,int, read, int, char *, size_t)
SYS_CALL_3(9 ,int, write, int, const char *, size_t)
SYS_CALL_1(10,int, close, int)
SYS_CALL_0(11,int, getpid)
SYS_CALL_3(12,int, signal, SignalType, uint64_t, pid_t)
SYS_CALL_1(13,int, create_process_from_file, const char *)
SYS_CALL_2(14,int, get_attr, int, FILE*)
SYS_CALL_0(15,int, real_exec)
SYS_CALL_1(16,int, exec, const char *)
SYS_CALL_1(17,int, signal_register, SignalType)
SYS_CALL_1(18,int, signal_unregister, SignalType)
SYS_CALL_1(19,int, unimplemented_syscall, const char *)
SYS_CALL_2(20,int, fstat, int, kStat*)
SYS_CALL_1(21, void*, sbrk, int)
SYS_CALL_2(22, int,link, const char *, const char *)
SYS_CALL_1(23, int, unlink, const char *)
SYS_CALL_3(24, int, lseek, int, int, int)
SYS_CALL_3(25, int, getdent, int, char *, int)
SYS_CALL_1(26, int, chdir, const char *)
SYS_CALL_2(27, int, getcwd, char *, int)

enum {
    SYS_FUNC_PRINTMSG = 0,
    SYS_FUNC_END
};

#ifdef __cplusplus
}
#endif
