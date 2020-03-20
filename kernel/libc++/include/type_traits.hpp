//
// Created by wmj on 3/20/20.
//

#ifndef OS_TYPE_TRAITS_HPP
#define OS_TYPE_TRAITS_HPP

namespace std {
    template <typename T> struct remove_reference {
        T type;
    };
    using remove_reference_t = typename remove_reference<T>::type;
}

#endif //OS_TYPE_TRAITS_HPP
