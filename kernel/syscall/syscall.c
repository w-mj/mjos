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
        (void*)do_print_msg,
        (void*)do_create_process,
        (void*)do_send_message,
        (void*)do_read_message,
        (void*)do_func_register,
        (void*)do_quit_thread,
        (void*)do_open,
        (void*)do_read,
        (void*)do_write
};

