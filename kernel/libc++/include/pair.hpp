//
// Created by wmj on 3/24/20.
//

#ifndef OS_PAIR_HPP
#define OS_PAIR_HPP

namespace os {
    template <typename A, typename B>
    class pair {
    public:
        pair(A a, B b): first(a), second(b) {}
        A first;
        B second;
    };

    template <typename A, typename B>
    pair<A, B> make_pair(A a, B b) {
        return pair<A, B>(a, b);
    }
}
#endif //OS_PAIR_HPP
