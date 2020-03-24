//
// Created by wmj on 3/20/20.
//

#ifndef OS_LRU_HPP
#define OS_LRU_HPP


#include <stddef.h>
#include <list.hpp>
#include <pair.hpp>

namespace std {
    template <typename K, typename T>
    class LRU {
    public:
        // TODO: 用函数对象代替函数指针
        typedef T(*constrcut_t)(K);
        typedef void(*destruct_t)(K, T);
        typedef void(*swap_out_t)(K, T, bool);
    private:
        struct _Wrapper {
            K key;
            T data;
            int cnt = 0;
            bool dirty = false;
        };
        list <_Wrapper> pool;
        size_t max_size;
        constrcut_t on_construct;
        destruct_t  on_destruct;
        swap_out_t on_swap_out;


    public:
        LRU(size_t sz,
            constrcut_t constructor,
            destruct_t destructor = [](K k, T t){},
            swap_out_t swap_out = [](K k, T t, bool dirty){}
            ):
            max_size(sz),
            on_construct(constructor),
            on_destruct(destructor),
            on_swap_out(swap_out)
            {}

            // 从缓存池中取得一个缓存
            // ret: pair<缓存, 这个缓存之前被访问到的次数>
        pair<T, int> get(K key, bool set_dirty=false) {
            // 在链表中线性查找K，可以优化
            for (auto it = pool.begin(); it != pool.end(); it++) {
                if (it->key == key) {
                    pool.move(it, pool.begin());  // 把找到的item移动到链表头
                    it->cnt++;
                    if (set_dirty) {
                        it->dirty = true;
                    }
                    return make_pair(it->data, it->cnt - 1);
                }
            }

            if (pool.size() < max_size) {
                //  未达到最大值，新建一个缓存
                _Wrapper wrapper;
                wrapper.data = on_construct(key);
                wrapper.key = key;
                wrapper.cnt = 1;
                wrapper.dirty = set_dirty;
                pool.push_front(wrapper);
                return make_pair(wrapper.data, 0);
            } else {
                //  置换处于链表尾部的缓存
                auto end = pool.end();
                end--;
                on_swap_out(end->key, end->data, end->dirty);
                end->data = on_construct(key);
                end->cnt = 1;
                end->key = key;
                end->dirty = set_dirty;
                pool.move(end, pool.begin());
                return make_pair(end->data, 0);
            }
        }

        ~LRU() {
            for (auto x: pool) {
                on_destruct(x.key, x.data);
            }
        }
    };
}

#endif //OS_LRU_HPP
