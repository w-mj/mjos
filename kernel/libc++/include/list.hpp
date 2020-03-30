//
// Created by wmj on 3/19/20.
//

#ifndef OS_LIST_H
#define OS_LIST_H


#include <stddef.h>
namespace os {

template <class T>
class list {
private:
    struct _Wrapper {
        T data;
        _Wrapper* next = nullptr;
        _Wrapper* prev = nullptr;
        _Wrapper() = default;
        explicit _Wrapper(T d): data(d) {}
    };
    _Wrapper head;
    size_t _size = 0;
public:
    class iterator;
    list() {
        head.next = &head;
        head.prev = &head;
    };
    ~list() {
        clear();
    }

    size_t size() {
        return _size;
    };

    void clear() {
        _Wrapper *cur = head.next;
        while (cur != &head) {
            _Wrapper *t = cur->next;
            delete cur;
            cur = t;
        }
    }

    void push_back(const T& data) {
        insert(end(), data);
    }

    void push_front(const T& data) {
        insert(begin(), data);
    }

    // 删除列表中的一个元素
    // ret true:  成功删除
    //     false: 未找到该元素
    bool remove(const T& data) {
        for (auto iter = begin(); iter != end(); iter++) {
            _Wrapper *x = iter._current;
            if (x->data == data) {
                x->prev->next = x->next;
                x->next->prev = x->prev;
                delete x;
                return true;
            }
        }
        return false;
    }

    // 从from移动到to前面
    // 相当于
    // auto value = *from;
    // erase(from);
    // insert(to, value);
    void move(const iterator &from, const iterator &to) {
        if (from == to) {
            return;
        }
        _Wrapper *from_wrapper = from._current;
        _Wrapper *to_wrapper   = to._current;
        // 断开from的连接
        if (from_wrapper->next != nullptr) {
            from_wrapper->prev = from_wrapper->next->prev;
        }
        if (from_wrapper->prev != nullptr) {
            from_wrapper->next = from_wrapper->prev->next;
        }
        // 将from插入to前面
        from_wrapper->prev = to_wrapper->prev;
        from_wrapper->next = to_wrapper;
        to_wrapper->prev->next = from_wrapper;
        to_wrapper->prev = from_wrapper;
    }

    void erase(const iterator& iter) {
        if (iter._current == &head) {
            return;
        }
        _Wrapper *wrapper = iter._current;
        wrapper->prev->next = wrapper->next;
        wrapper->next->prev = wrapper->prev;
        iter._current = wrapper->next;
        delete wrapper;
    }

    // 在iter的前面插入
    void insert(const iterator& iter, const T& data) {
        auto *wrapper = new _Wrapper(data);
        auto *behind = iter._current;
        iterator new_iter(wrapper);
        move(new_iter, iter);
//        wrapper->next = behind;
//        wrapper->prev = behind->prev;
//        behind->prev->next = wrapper;
//        behind->prev = wrapper;
        _size++;
    }

    T& front() {
        return head.next->data;
    }

    T& last() {
        return head.prev->data;
    }

//    T& operator[] (int index) {
//        auto iter = begin();
//        for (; iter != end() && index; iter++, index--)
//            ;
//        return iter._current->data;
//    }

    class iterator  {
    friend class list;
    private:
        _Wrapper* _current;
    public:
        explicit iterator(_Wrapper *_current) {
            this->_current = _current;
        }
        iterator &operator++() {
            _current = _current->next;
            return *this;
        }
        iterator &operator--() {
            _current = _current->prev;
            return *this;
        }
        const iterator &operator++(int) {
            _current = _current->next;
            return *this;
        }
        const iterator &operator--(int) {
            _current = _current->prev;
            return *this;
        }
        bool operator!=(const iterator &ano) const {
            return _current != ano._current;
        }
        bool operator==(const iterator &ano) const {
            return ! this->operator!=(ano);
        }
        T& operator*() {
            return _current->data;
        }
        T *operator->() {
            return &_current->data;
        }
    };
    iterator begin() {
        return iterator(head.next);
    }
    iterator end() {
        return iterator(&head);
    }
};
}

#endif //OS_LIST_H
