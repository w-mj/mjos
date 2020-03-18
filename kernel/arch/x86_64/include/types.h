#pragma once
#ifdef __cplusplus
extern "C" {
#endif


typedef signed char _i8;
typedef unsigned char _u8;
typedef signed short _i16;
typedef unsigned short _u16;
typedef signed int _i32;
typedef unsigned int _u32;
typedef signed long long _i64;
typedef unsigned long long _u64;
typedef _u32 _I;
typedef _u32 size_t;
typedef _i32 ptrdiff_t;
typedef char _c;
typedef _c* _s;
typedef unsigned char* _p;
typedef unsigned long addr_t;
typedef _u64 usize;

#define NOADDR ((usize)-1)
#ifndef __cplusplus
#define bool _u8
#define true 1
#define false 0
#endif

#define def_type(x)  \
	typedef _i##x i##x; \
	typedef _u##x u##x;

def_type(8)
def_type(16)
def_type(32)
def_type(64)

#ifndef __cplusplus
#define NULL ((void*)0)
#else
#define NULL nullptr
#endif

typedef u32 pfn_t;
typedef u16 pid_t;

#ifdef __cplusplus
}
#endif
