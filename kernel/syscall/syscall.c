#include <delog.h>
#include <process/process.h>
#include <cpu.h>
#include <syscall.h>
#include <string.h>
#include <fs/syscall_handler.h>
#include <process/process_helper.hpp>
#include <memory/sbrk.h>


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

int do_unimplemented_syscall(const char *name) {
    loge("syscall %s is not implemented yet.", name);
    die();
    return 0;
}

void *syscall_tbl[256] = {
        /* 0 */  (void*)do_kernel_return,
        /* 1 */  (void*)do_print_msg,
        /* 2 */  (void*)do_create_process,
        /* 3 */  (void*)do_send_message,
        /* 4 */  (void*)do_read_message,
        /* 5 */  (void*)do_func_register,
        /* 6 */  (void*)do_quit_thread,
        /* 7 */  (void*)do_open,
        /* 8 */  (void*)do_read,
        /* 9 */  (void*)do_write,
        /* 10 */ (void*)do_close,
        /* 11 */ (void*)do_getpid,
        /* 12 */ (void*)do_signal,
        /* 13 */ (void*)do_create_process_from_file,
        /* 14 */ (void*)do_get_attr,
        /* 15 */ (void*)do_real_exec,
        /* 16 */ (void*)do_exec,
        /* 17 */ (void*)do_signal_register,
        /* 18 */ (void*)do_signal_unregister,
        /* 19 */ (void*)do_unimplemented_syscall,
        /* 20 */ (void*)do_fstat,
        /* 21 */ (void*)do_sbrk,
        /* 22 */ (void*)do_link,
        /* 23 */ (void*)do_unlink,
        /* 24 */ (void*)do_lseek
};
