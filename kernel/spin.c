#include <atomic.h>
#include <spin.h>

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
