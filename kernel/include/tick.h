#pragma once
#include <types.h>
#include <list.h>
typedef int (* wdog_proc_t) (void * a1, void * a2, void * a3, void * a4);

typedef struct wdog {
    ListEntry   node;
    int         ticks;
    wdog_proc_t proc;
    void *      arg1;
    void *      arg2;
    void *      arg3;
    void *      arg4;
} wdog_t;

#define WDOG_INIT       ((wdog_t) { DLNODE_INIT, 0, NULL, 0,0,0,0 })

#define WAIT_FOREVER    ((int) -1)
#define NO_WAIT         ((int) -2)

extern void  wdog_start(wdog_t * wd, int ticks, void * proc,
                        void * a1, void * a2, void * a3, void * a4);
extern void  wdog_stop (wdog_t * wd);

extern void  tick_proc ();
extern usize tick_get  ();
extern void  tick_delay(int ticks);


