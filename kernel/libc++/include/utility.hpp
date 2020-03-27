//
// Created by wmj on 3/20/20.
//

#ifndef OS_UTILITY_HPP
#define OS_UTILITY_HPP

#include <type_traits.hpp>

namespace os {

    template <class _Ty>
    constexpr remove_reference_t <_Ty> &&move(_Ty &&_Arg) noexcept { // forward _Arg as movable
        return static_cast<remove_reference_t <_Ty> &&>(_Arg);
    }

    template <class T>
    inline T&& forward(typename os::remove_reference<T>::type& t) noexcept {
        return static_cast<T&&>(t);
    }

    template <class T>
    inline T&& forward(typename os::remove_reference<T>::type&& t) noexcept {
        static_assert(!os::is_lvalue_reference<T>::value,
                      "Can not forward an rvalue as an lvalue.");
        return static_cast<T&&>(t);
    }

//    template<bool> // generic
//    struct static_assert;

//    template<>
//    struct static_assert<true>{};
}

#endif //OS_UTILITY_HPP
