#include <process/process.h>
#include <list.h>
#include <cpu.h>
#include <delog.h>

__PERCPU ThreadDescriber *current = NULL;
static ListEntry running_threads = LIST_INIT;

void add_thread_to_running(ThreadDescriber *thread) {
	thread->state = THREAD_RUNNING;
	list_add(&thread->next, &running_threads);
}

void schedule() {
	if (list_empty(&running_threads)) {
		return;
	}
	ListEntry *to_run = list_pop_head(&running_threads);
	ThreadDescriber *next = list_entry(to_run, ThreadDescriber, next);
	if (thiscpu_var(current) == NULL) {
		thiscpu_var(current) = next;
		prepare_switch(NULL, next);
	} else {
		list_add_tail(&thiscpu_var(current)->next, &running_threads);
		prepare_switch(thiscpu_var(current), next);
		thiscpu_var(current) = next;
	}
}
