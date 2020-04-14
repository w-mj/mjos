#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

typedef struct __Spin {
	i32 tkt;
	i32 svc;
} Spin;


void raw_spin_take(Spin *lock);
void raw_spin_give(Spin *lock);
u32  irq_spin_take(Spin *lock);
void irq_spin_give(Spin *lock, u32 key);

#define SPIN_INIT ((Spin) {0, 0})

#ifdef __cplusplus
}
#endif
