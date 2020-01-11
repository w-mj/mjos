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
