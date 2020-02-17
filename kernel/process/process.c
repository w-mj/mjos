#include "process_struct.h"
#include <string.h>
#include <list.h>
#include <base.h>
#include <delog.h>
#include <asm.h>
#include <memory/page.h>
#include <memory/kmalloc.h>

static ListEntry running_list;
static u8 *pid_bitmap = NULL;

void process_init() {
	logd("init process");
	pid_bitmap = (u8*)kernel_page_alloc();
	memset(pid_bitmap, 0, PAGESIZE);
	list_init(&running_list);
}

static u16 find_usable_pid() {
	u16 pid = 0;
	while(pid < 32767) {
		if (chk_bitmap(pid_bitmap, pid)) {
			set_bitmap(pid_bitmap, pid);
			return pid;
		}
		pid++;
	}
	return (u16)-1;
}

void create_process() {
	u16 pid = find_usable_pid();
	if (pid == (u16)-1) {
		loge("no usable pid.");
		die();
	}
}

