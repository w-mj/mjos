#include <types.h>
#include <spin.h>
#include <delog.h>
#include <tick.h>
#include <cpu.h>
#include <atomic.h>

// a giant lock (tick_q.lock) is used to guard all watch dog operations
// so spinlock is not needed inside wdog_t structure

// `wd->proc == NULL` means this wdog is not in tick_q

typedef struct tick_q {
    Spin spin;
    ListEntry q;
} tick_q_t;

static tick_q_t tick_q  = { SPIN_INIT, LIST_INIT };
static size_t tick_count = 0;

void wdog_start(wdog_t * wd, int ticks, void * proc,
                void * a1, void * a2, void * a3, void * a4) {
    assert(NULL != wd);
    assert(NULL != proc);
    assert(ticks >= 0);
    assert(ticks != WAIT_FOREVER);

    u32 key = irq_spin_take(&tick_q.spin);
    if (NULL != wd->proc) {
        irq_spin_give(&tick_q.spin, key);
        return;
    }

    wd->proc = proc;
    wd->arg1 = a1;
    wd->arg2 = a2;
    wd->arg3 = a3;
    wd->arg4 = a4;
    ticks += 1;

    ListEntry *node = tick_q.q.next;
    wdog_t    *wdog = list_entry(node, wdog_t, node);

    while ((&tick_q.q != node) && (wdog->ticks <= ticks)) {
        ticks -= wdog->ticks;
        node   = node->next;
        wdog   = list_entry(node, wdog_t, node);
    }

    wd->ticks = ticks;
    if (wdog) {
        wdog->ticks -= ticks;
    }

    list_insert_before(&wd->node, node);
    irq_spin_give(&tick_q.spin, key);
}

void wdog_stop(wdog_t * wd) {
    u32 key = irq_spin_take(&tick_q.spin);

    if (NULL != wd->proc) {
        ListEntry *node = wd->node.next;
        wdog_t   * next = list_entry(node, wdog_t, node);
        list_remove(&wd->node);
        wd->proc = NULL;
        if (NULL != node) {
            next->ticks += wd->ticks;
        }
    }

    irq_spin_give(&tick_q.spin, key);
}

// clock interrupt handler
void tick_proc() {
    if (0 == cpu_index()) {
        atomic_inc(&tick_count);

        u32 key = irq_spin_take(&tick_q.spin);
        ListEntry *node = tick_q.q.next;
        wdog_t   * wdog = list_entry(node, wdog_t, node);
        if (NULL != node) {
            --wdog->ticks;
        }

        while ((NULL != node) && (wdog->ticks <= 0)) {
            list_pop_head(&tick_q.q);
            wdog_proc_t proc = wdog->proc;
            wdog->proc = NULL;

            // during wdog execution, contention of tick_q is released
            // so we can start (another or the same) wdog inside proc
            irq_spin_give(&tick_q.spin, key);
            proc(wdog->arg1, wdog->arg2, wdog->arg3, wdog->arg4);
            key = irq_spin_take(&tick_q.spin);

            node = tick_q.q.next;
            wdog = list_entry(node, wdog_t, node);
        }

        irq_spin_give(&tick_q.spin, key);
    }
	loge("not implement");
    // sched_tick();
}

usize tick_get() {
    return tick_count;
}

// busy wait
void tick_delay(int ticks) {
    usize start = tick_count;
    while ((tick_count - start) < (usize) ticks) {
        relax();
    }
}

