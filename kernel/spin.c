#include <atomic.h>
#include <spin.h>
#include <process/scheduler.h>

void raw_spin_take(Spin *lock) {
    int tkt = atomic32_inc(&lock->tkt);
    while (atomic32_get(&lock->svc) != tkt) {
        relax();
    }
}

void raw_spin_give(Spin *lock) {
    atomic32_inc(&lock->svc);
}

u32 irq_spin_take(Spin *lock) {
    u32 key = int_lock();
    int tkt = atomic32_inc(&lock->tkt);
    while (atomic32_get(&lock->svc) != tkt) {
        int_unlock(key);
        relax();
        key = int_lock();
    }
    return key;
}

void irq_spin_give(Spin *lock, u32 key) {
    atomic32_inc(&lock->svc);
    int_unlock(key);
}

int do_passeren(Semaphore *sem) {
    i32 i = atomic32_dec(sem);
    if (i <= 0) {
        // 如果可用信号量不足，则挂起当前线程
        thread_wait(ThreadWaitMutex, (u64)sem);
    }
    return i;
}

int do_verhoog(Semaphore *sem) {
    i32 i = atomic32_inc(sem);
    if (i < 0) {
        // 有被挂起的线程，唤醒
        thread_resume(ThreadWaitMutex, (u64)sem);
    }
    return i;
}
