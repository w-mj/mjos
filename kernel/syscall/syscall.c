#include <delog.h>
#include <process/process.h>
#include <process/ipc.h>
#include <cpu.h>
#include <syscall.h>
#include <string.h>
#include <fs/syscall_handler.h>

pid_t sysfunc_tbl[256] = {};

int do_print_msg(const char *msg) {
    if (sysfunc_tbl[SYS_FUNC_PRINTMSG] == 0) {
        loge("system function print message is not registered");
        return -1;
    }
    return do_send_message(sysfunc_tbl[SYS_FUNC_PRINTMSG], msg, strlen(msg));
}

int do_func_register(int funnum) {
    if (funnum < 0 || funnum >= SYS_FUNC_END) {
        loge("invalid system function number");
        return -1;
    }
    sysfunc_tbl[funnum] = thiscpu_var(current)->process->pid;
    return funnum;
}

void *syscall_tbl[256] = {
        NULL,
        (void*)do_print_msg,         /* 1 */
        (void*)do_create_process,    /* 2 */
        (void*)do_send_message,      /* 3 */
        (void*)do_read_message,      /* 4 */
        (void*)do_func_register,     /* 5 */
        (void*)do_quit_thread,       /* 6 */
        (void*)do_open,              /* 7 */
        (void*)do_read,              /* 8 */
        (void*)do_write,             /* 9 */
        (void*)do_close,             /* 10 */
        (void*)do_getpid             /* 11 */
};

