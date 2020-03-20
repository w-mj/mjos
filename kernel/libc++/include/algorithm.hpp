//
// Created by wmj on 3/19/20.
//

#ifndef OS_ALGORITHM_H
#define OS_ALGORITHM_H

#include <function.hpp>

namespace std {
    template <typename T>
    T min(T d1, T d2) {
        if (d1 < d2)
            return d1;
        return d2;
    }

    template <typename T, typename... Args>
    T min(T d1, Args ... args) {
        return min(d1, min(args...));
    }

// 从低位数第一个1，返回值为sizeof(T) * 8时说明全是0
    template<typename T> static inline _u8 lowest_1(T n) {
        _u8 i = 0;
        _u8 bits_cnt = sizeof(T) << 3;
        T mask = 1;
        while (i < bits_cnt) {
            if (mask & n)
                return i;
            i++;
            mask <<= 1;
        }
        return i;
    }

    template<typename T> static inline _u8 highest_1(T n) {
        _u8 bits_cnt = sizeof(T) << 3;
        int i = bits_cnt - 1;
        T mask = 1 << (bits_cnt - 1);
        while (i < bits_cnt) {
            if (mask & n)
                return i;
            i--;
            mask >>= 1;
        }
        return bits_cnt;
    }

    template<typename T> static inline _u8 lowest_0(T n) {
        return lowest_1(~n);
    }

    template<typename T> static inline _u8 highest_0(T n) {
        return highest_1(~n);
    }


    template <typename T, typename D>
    T find_if(T begin, T end, function<bool(D)> pred) {
        while (begin != end) {
            if (pred(*begin)) {
                return begin;
            }
            ++begin;
        }
    }

}

#endif //OS_ALGORITHM_H
