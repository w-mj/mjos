#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <process/process.h>
#include <stdint.h>
#include <sys/types.h>
#include <process/signal.h>
#include <asm.h>
#include <stdio.h>

struct stat;
struct tms;
struct timeval;
struct timezone;

#define PREPARE(n) ASM("movq $"#n", %r9"); \
    do {\
        int cs;\
        ASM("mov %%cs, %0": "=r"(cs)); \
        if ((cs & 0x3) == 0) { \
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

/*
 * void _exit();
int close(int file);
char **environ; pointer to array of char * strings that define the current environment variables
int execve(char *name, char **argv, char **env);
int fork();
int fstat(int file, struct stat *st);
int getpid();
int isatty(int file);
int kill(int pid, int sig);
int link(char *old, char *new);
int lseek(int file, int ptr, int dir);
int open(const char *name, int flags, ...);
int read(int file, char *ptr, int len);
caddr_t sbrk(int incr);
int stat(const char *file, struct stat *st);
clock_t times(struct tms *buf);
int unlink(char *name);
int wait(int *status);
int write(int file, char *ptr, int len);
int gettimeofday(struct timeval *p, struct timezone *z);
 */

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
SYS_CALL_1(17, signal_register, SignalType)
SYS_CALL_1(18, signal_unregister, SignalType)

/*
SYS_CALL_3(19, execve,char *, char **, char **)
SYS_CALL_0(20, fork)
SYS_CALL_2(21, fstat, int, struct stat*)
SYS_CALL_1(22, isatty, int)
SYS_CALL_2(23, kill, int, int)
SYS_CALL_2(24, link, char *, char *)
SYS_CALL_3(25, lseek, int, int, int)
// 26 open(const char *, int, ...)
SYS_CALL_1(27, sbrk, int)  // ret caddr_t
SYS_CALL_2(28, stat, const char*, struct stat *);
SYS_CALL_1(29, times, struct tms*) // ret clock_t
SYS_CALL_1(30, wait, int*)
SYS_CALL_1(31, unlink, char *)
SYS_CALL_2(32, gettimeofday, struct timeval*, struct timezone *)
*/

#define _exit sys_quit_thread
#define close sys_close

enum {
    SYS_FUNC_PRINTMSG = 0,
    SYS_FUNC_END
};

#ifdef __cplusplus
}
#endif
