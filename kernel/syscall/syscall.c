#include <delog.h>
#include <process/process.h>
#include <process/ipc.h>
#include <cpu.h>
#include <syscall.h>
#include <string.h>

pid_t sysfunc_tbl[256] = {};

int do_print_msg(char *msg) {
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
        do_print_msg,
        do_create_process,
        do_send_message,
        do_read_message,
        do_func_register,
        do_quit_thread,
};

