#pragma once

#define _getva_T_(P, params, _T_) do {P = (_T_)va_arg(params, _T_); }while(0)
#define _getva_S(s, params) _getva_T_(s, params, _s)
#define _getva_I(s, params) _getva_T_(s, params, _I)
#define _getva_P(s, params) _getva_T_(s, params, _p)
#define _getva_double(s, params) _getva_T_(s, params, double)
#define _getva_I32(s, params) _getva_T_(s, params, _i32)

#define _BITS_SIZE(n) (1 << (n))
#define _BITS_MASK(n) (_BITS_SIZE(n) - 1)
#define _BITSET(n) _BITS_SIZE(n)
#define _get_bit(d, bit) ((d) & _BITSET(bit))
#define _set_bit(d, bit) ((d) |= _BITSET(bit))
#define _clr_bit(d, bit) ((d) &= (~(_BITSET(bit))))
#define _chk_bit(d, bit) _get_bit(d, bit)

#define _rshf(a, n) ((a) << (n))
#define _lshf(a, n) ((a) >> (n))
#define _Lclr(a, n) ((a) & (~(_BITS_MASK(n))))
#define _Hclr(a, n) _rshf(_lshf(a, n), n)
#define _LNclr(a, n) (((a) & (_BITS_MASK(n))))

#define _Inext(pI, i) ((pI) + (i))->_next

#define nullptr NULL
#define null NULL


#define ASM(...)                __asm__ volatile(__VA_ARGS__)
#define TYPE(x)                 __typeof__(x)

#define CLZ32(x)                __builtin_clz(x)
#define CTZ32(x)                __builtin_ctz(x)
#define CLZ64(x)                __builtin_clzll(x)
#define CTZ64(x)                __builtin_ctzll(x)
#define CLZL(x)                 __builtin_clzl(x)
#define CTZL(x)                 __builtin_ctzl(x)
#define MAX(x, y)               (((x) > (y)) ? (x) : (y))
#define MIN(x, y)               (((x) < (y)) ? (x) : (y))

#define ROUND_UP(x, align)      (((usize) (x) + ((align)-1)) & ~((align)-1))
#define ROUND_DOWN(x, align)    ( (usize) (x)                & ~((align)-1))
#define IS_ALIGNED(x, align)    (((usize) (x) & ((align)-1)) == 0)

/* 通过结构体中某个成员的地址找到结构体的地址
 * ptr: 结构体某个成员的地址
 * type: 结构体的类型
 * member: 这个成员在结构体中的名称
 */
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
