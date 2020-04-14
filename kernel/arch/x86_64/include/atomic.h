#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "asm.h"

i32 atomic32_get(i32 * p);
i32 atomic32_set(i32 * p, i32 x);
i32 atomic32_inc(i32 * p);
i32 atomic32_dec(i32 * p);
i32 atomic32_cas(i32 * p, i32 x, i32 y);

i64 atomic64_get(i64 * p);
i64 atomic64_set(i64 * p, i64 x);
i64 atomic64_inc(i64 * p);
i64 atomic64_dec(i64 * p);
i64 atomic64_cas(i64 * p, i64 x, i64 y);


#define atomic_get(p)       atomic64_get((i64 *) p)
#define atomic_set(p, x)    atomic64_set((i64 *) p, (i64) x)
#define atomic_inc(p)       atomic64_inc((i64 *) p)
#define atomic_dec(p)       atomic64_dec((i64 *) p)
#define atomic_cas(p, x, y) atomic64_cas((i64 *) p, (i64) x, (i64) y)

#ifdef __cplusplus
}
#endif
