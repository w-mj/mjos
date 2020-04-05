#include <process/process.h>
#include <list.h>
#include <cpu.h>
#include <delog.h>

__PERCPU ThreadDescriptor *current = NULL;
static ListEntry running_threads;

void scheduler_init() {
	list_init(&running_threads);
}

void add_thread_to_running(ThreadDescriptor *thread) {
	thread->state = THREAD_RUNNING;
	list_add(&thread->next, &running_threads);
}

void remove_thread_from_running(ThreadDescriptor *thread) {
	thread->state = THREAD_STOPPED;
	list_remove(&thread->next);
}

#define plist \
	_sL(&running_threads); \
	_sL(running_threads.next); \
	_sL(running_threads.next->next); \
	_sL(running_threads.next->next->next);

bool schedule() {
	if (list_empty(&running_threads)) {
		return false;
	}
	// ListEntry *c;
	// plist
	ListEntry *to_run = list_pop_head(&running_threads);
	assert(to_run != NULL);
	// assert(list_empty(&running_threads));
	ThreadDescriptor *next = list_entry(to_run, ThreadDescriptor, next);
	if (thiscpu_var(current) == NULL) {
		thiscpu_var(current) = next;
		prepare_switch(NULL, next);
		set_rsp0((u64) next->rsp0);
		load_tid_next(next);
	} else {
		// _sL(&thiscpu_var(current)->next);
		// _sL(thiscpu_var(current)->next.prev);
		// _sL(thiscpu_var(current)->next.next);
		list_add_tail(&next->next, &running_threads);
		//plist
		prepare_switch(thiscpu_var(current), next);
		thiscpu_var(current) = next;
		set_rsp0((u64) next->rsp0);
        thread_switch();
	}
	return true;
}

void before_schedule_in() {
    signalCheck();
}
