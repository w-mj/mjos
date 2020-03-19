//
// Created by wmj on 3/19/20.
//

#ifndef OS_LIST_H
#define OS_LIST_H

#include <stddef.h>
namespace std {

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

    list() = default;
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

    void push_back(T data) {
        auto *w = new _Wrapper(data);
        w->next = &head;
        w->prev = head.prev;
        head.prev->next = w;
        head.prev = w;
    }

    void remove(T data) {
        for (auto iter = begin(); iter != end(); iter++) {
            _Wrapper *x = iter._current;
            if (x->data == data) {
                x->prev->next = x->next;
                x->next->prev = x->prev;
                delete x;
                return;
            }
        }
    }

    T front() {
        return head.next->data;
    }

    T operator[] (int index) {
        auto iter = begin();
        for (; iter != end() && index; iter++, index--)
            ;
        return iter._current->data;
    }

    class iterator  {
    friend class list;
    private:
        _Wrapper* _current;
    public:
        iterator(_Wrapper *_current) {
            this->_current = _current;
        }
        iterator &operator++() {
            _current = _current->next;
            return *this;
        }
        const iterator &operator++(int) {
            _current = _current->next;
            return *this;
        }
        bool operator!=(const iterator &ano) {
            return _current != ano._current;
        }
        T operator*() {
            return _current->data;
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
