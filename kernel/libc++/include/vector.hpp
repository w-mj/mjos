//
// Created by wmj on 3/19/20.
//

#ifndef OS_VECTOR_HPP
#define OS_VECTOR_HPP


#include <stddef.h>
#include <stdlib.h>
#include <string.h>

namespace os {
    template <typename T>
    class vector {
    private:
        T *data = nullptr;
        size_t item_size = 0;
        size_t array_size = 0;
        void extend(size_t target) {
            if (data == nullptr) {
                data = (T*)calloc(target, sizeof(T));
            } else {
                data = (T*)realloc((void*)data, target * sizeof(T));
            }
            array_size = target;
        }
        void extend() {
            if (item_size == array_size) {
                if (array_size == 0) {
                    extend(8);
                } else {
                    extend(array_size * 2);
                }
            }
        }
    public:
        vector() = default;
        explicit vector(size_t size) {
            extend(size);
        }
        vector(const vector<T> &ano) {
            item_size = ano.size();
            extend(item_size);
            memcpy(data, ano.data, item_size * sizeof(T));
        }
//        vector<T> &operator=(const vector<T> &ano) {
//            return *this;
//        }
        void push_back(T item) {
            extend();
            data[item_size] = item;
            item_size++;
        }

        T pop_back() {
            item_size--;
            return data[item_size];
        }

        T& operator[](int i) {
            return data[i];
        }
        const T& operator[](int i) const{
            return data[i];
        }
        size_t size() const {
            return item_size;
        }
        bool empty() const {
            return size() == 0;
        }
        T& front() {
            return this->operator[](0);
        }

        class iterator {
            T *cur;
        public:
            explicit iterator(T *t) :cur(t){}
            bool operator!=(const iterator &ano) {
                return cur != ano.cur;
            }
//            const iterator & operator++(int){
//                cur++;
//                return *this;
//            }
            iterator &operator++() {
                cur++;
                return *this;
            }
            T operator*() {
                return *cur;
            }
        };

        iterator begin() {
            return iterator(data);
        }
        iterator end() {
            return iterator(data + item_size);
        }
    };
}

#endif //OS_VECTOR_HPP
