#include "process_struct.h"
#include <memory.h>
#include <string.h>
#include <list.h>

static ListEntry running_list;
static u8 *pid_bitmap = NULL;

void process_init() {
	pid_bitmap = (u8*)kernel_page_alloc();
	memset(pid_bitmap, 0, PAGESIZE);
	list_init(&running_list);
}

void create_process() {
	
}

