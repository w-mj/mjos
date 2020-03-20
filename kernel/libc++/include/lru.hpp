//
// Created by wmj on 3/20/20.
//

#ifndef OS_LRU_HPP
#define OS_LRU_HPP

#include <stddef.h>
#include <list.hpp>

namespace std {
    template <typename K, typename T>
    class LRU {
    private:
        struct _Wrapper {
            K key;
            T data;
            int cnt = 0;
        };
        list <_Wrapper> pool;
        size_t size;
    public:
        LRU(size_t sz, T (*fn)()) {
            size = sz;
            while (sz--) {}
            pool.push_back(fn());
        }
    };
}

#endif //OS_LRU_HPP
