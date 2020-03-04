#include <delog.h>
#include <process/process.h>

void sc_print_msg(char *msg) {
	logi("%s", msg);
}

void *syscall_tbl[256] = {
	NULL,
	sc_print_msg,
	create_process
};
