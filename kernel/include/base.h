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
#define _LNclr(a, n) ((a & (_BITS_MASK(n))))

#define _Inext(pI, i) ((pI) + (i))->_next

#define nullptr NULL
#define null NULL
