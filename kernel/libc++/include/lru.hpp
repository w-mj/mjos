//
// Created by wmj on 3/20/20.
//

#ifndef OS_LRU_HPP
#define OS_LRU_HPP

#include <stddef.h>
#include <list.hpp>
#include <function.hpp>

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
        size_t max_size;
        function <T(K)> fn;
    public:
        LRU(size_t sz, function<T(K)> fn): max_size(sz), fn(fn) {}
        T get(K key) {
            for (auto it = pool.begin(); it != pool.end(); it++) {
                if ((*it).key == key) {
                    pool.move(it, pool.begin());
                    (*it).cnt++;
                    return *it;
                }
                if (pool.size() < max_size) {
                    _Wrapper wrapper;
                    wrapper.data = fn(key);
                    wrapper.key = key;
                    wrapper.cnt = 1;
                    pool.push_front(wrapper);
                    return wrapper.data;
                } else {
                    auto end = pool.end();
                    end--;
                    (*end).data = fn(key);
                    (*end).cnt = 0;
                    (*end).key = key;
                    pool.move(end, pool.begin());
                    return (*end).data;
                }
            }
        }
    };
}

#endif //OS_LRU_HPP
