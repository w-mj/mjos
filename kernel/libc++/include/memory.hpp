//
// Created by wmj on 3/21/20.
//

#ifndef OS_MEMORY_HPP
#define OS_MEMORY_HPP

#include <type_traits.hpp>

namespace std {
    template<typename T>
    class unique_ptr {
    private:
        T* _ptr;
    public:
        unique_ptr(T& t) {
            _ptr = &t;
        }
        unique_ptr(unique_ptr<T>&& ano) {
            _ptr = std::move(ano._ptr);
            ano._ptr = nullptr;
        }
        ~unique_ptr() {
            delete _ptr;
        }
        unique_ptr<T>& operator=(unique_ptr<T>&& ano) {
            if (this == ano) return *this;
            _ptr = std::move(ano._ptr);
            ano._ptr = nullptr;
            return *this;
        }

        T* get() {
            return _ptr;
        }

        void reset(T* ptr) {
            T* old_ptr = _ptr;
            _ptr = ptr;
            delete old_ptr;
        }

        bool operator != (T* ptr) {
            return _ptr != ptr;
        }

        unique_ptr(const unique_ptr<T>& ano) = delete;
        unique_ptr<T>& operator=(const unique_ptr<T>& ano) = delete;
    };
}

#endif //OS_MEMORY_HPP
