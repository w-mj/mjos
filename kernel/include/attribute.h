#pragma once


#define __UNUSED                __attribute__((unused))
#define __PACKED                __attribute__((packed))
#define __ALIGNED(x)            __attribute__((aligned(x)))
#define __SECTION(x)            __attribute__((section(x)))
#define __INIT                  __SECTION(".init.text")
#define __INITDATA              __SECTION(".init.data")
#define __PERCPU                __SECTION(".percpu")
#define __WEAK                  __attribute__((weak))
#define __PURE                  __attribute__((pure))
#define __NORETURN              __attribute__((noreturn))

#define __aligned(x) __attribute__((__aligned__(x)))
#define __section(S) __attribute__ ((__section__(#S)))
#define __page_aligned_data	__section(.data..page_aligned) __aligned(PAGESIZE)
#define __page_aligned_bss	__section(.bss..page_aligned) __aligned(PAGESIZE)

