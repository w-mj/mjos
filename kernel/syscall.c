#include <delog.h>
#include <process/process.h>
#include <process/ipc.h>

void sc_print_msg(char *msg) {
	logi("%s", msg);
}

void *syscall_tbl[256] = {
	NULL,
	sc_print_msg,
	do_sys_create_process,
	send_message,
	read_message
};
