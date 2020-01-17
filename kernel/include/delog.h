#pragma once

#include <types.h>
#include <stdarg.h>
#define _POS_INFOBITS (_I)1
#define _NPOS_INFOBITS (~_POS_INFOBITS)
#define mkstr(symbol) #symbol
enum {
    _INIT_INFO_DELOG = 0,
    _LOG_INFO_DELOG = 2,
    _DBG_INFO_DELOG = 4,
    _MAX_INFO_DELOG,
};
#define _pos_delog(type, ...) _wmj_delog((type)|_POS_INFOBITS, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define _Npos_delog(type, ...) _wmj_delog((type)&_NPOS_INFOBITS, __VA_ARGS__)
// #define init_delog() do{_Npos_delog(_INIT_INFO_DELOG, 0);} while(0)
#define init_delog(dbgname, logname) do{_Npos_delog(_INIT_INFO_DELOG, dbgname, logname);}while(0)
#if DELOG_MODE < 2
#define _debug_info(expstr, m, ...) _pos_delog(_DBG_INFO_DELOG, m, expstr, __VA_ARGS__)
#else
#define _debug_info(expstr, m, ...) do{}while(0)
#endif
#define _log_info(...) _Npos_delog(_LOG_INFO_DELOG, __VA_ARGS__)
#define _reg() _log_info("[%s] run!", __func__)

// 打印一段地址空间
#define _sa(exp, len) _debug_info(mkstr(exp), "a", exp, len)
// 以十进制打印无符号一个整数
#define _sI(exp) _debug_info(mkstr(exp), "I", exp)
// 以十六进制打印一个整数
#define _sx(exp) _debug_info(mkstr(exp), "x", (_u32)exp)
// 打印一个浮点数
// #define _sf(exp) _debug_info(mkstr(exp), "f", (double)exp)
// 打印一个字符串
#define _ss(exp) _debug_info(mkstr(exp), "s", (_s)exp)
// 打印一个变量的地址
#define _sp(exp) _debug_info(mkstr(exp), "p", &(exp))
// 打印一个字符
#define _sc(exp) _debug_info(mkstr(exp), "c", (_i32)(exp))
// 以十进制打印一个带符号整数
#define _si(exp) _debug_info(mkstr(exp), "d", (_i32)exp)
// 打印当前位置
#define _pos() _debug_info("", "n", "")

void _wmj_delog(_I type, ...);

/***
 * 将p所指位置连续len个字节的单元的值取出，以16进制放在str中，size为str的最大长度
 */
_I print_bin(_s str, _I size, _u8 *p, _I len);

