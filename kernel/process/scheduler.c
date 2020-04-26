#include <process/process.h>
#include <list.h>
#include <cpu.h>
#include <delog.h>

__PERCPU ThreadDescriptor *current = NULL;
static ListEntry running_threads;
static ListEntry blocked_threads;

void scheduler_init() {
	list_init(&running_threads);
	list_init(&blocked_threads);
}

void add_thread_to_running(ThreadDescriptor *thread) {
	thread->state = THREAD_RUNNING;
	list_add(&thread->next, &running_threads);
}

void remove_thread_from_running(ThreadDescriptor *thread) {
	thread->state = THREAD_BLOCKED;
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
    list_add_tail(&next->next, &running_threads);
	if (thiscpu_var(current) == NULL) {
		thiscpu_var(current) = next;
		prepare_switch(NULL, next);
		set_rsp0((u64) next->rsp0);
		load_tid_next(next);
	} else {
		// _sL(&thiscpu_var(current)->next);
		// _sL(thiscpu_var(current)->next.prev);
		// _sL(thiscpu_var(current)->next.next);
		//plist
		prepare_switch(thiscpu_var(current), next);
		thiscpu_var(current) = next;
		set_rsp0((u64) next->rsp0);
        thread_switch();
	}
	return true;
}

void before_schedule_in(SwitchContext *context) {
    signalCheck(context);
}

void thread_wait(enum ThreadWaitType type, u64 value) {
    ThreadDescriptor *thread = thiscpu_var(current);
    thread->waitType = type;
    thread->waitValue = value;
    remove_thread_from_running(thread);
    list_add(&thread->next, &blocked_threads);
    schedule();
}

static inline void resume(ThreadDescriptor *thread) {
    thread->waitType = ThreadRunning;
    list_remove(&thread->next);
    add_thread_to_running(thread);
}

void thread_resume(enum ThreadWaitType type, u64 value) {
    ListEntry *c = blocked_threads.next;
    while (c != &blocked_threads) {
        ThreadDescriptor *thread = list_entry(c, ThreadDescriptor, next);
        switch (type) {
            case ThreadWaitDMA:
            case ThreadWaitPid:
                if (thread->waitValue != value)
                    break;
                c = c->next;
                resume(thread);
                continue;
            case ThreadWaitMutex:
            case ThreadWaitFile:
                if (thread->waitValue == value) {
                    resume(thread);
                    return ;   // V原语一次只唤醒一个线程
                }
                break;
            default:
                loge("you should not be here");
                break;
        }
        c = c->next;
    }
}