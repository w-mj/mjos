//
// Created by wmj on 3/20/20.
//

#ifndef OS_UTILITY_HPP
#define OS_UTILITY_HPP

#include <type_traits.hpp>

namespace std {

    constexpr remove_reference_t <_Ty> &&move(_Ty &&_Arg) noexcept { // forward _Arg as movable
        return static_cast<remove_reference_t <_Ty> &&>(_Arg);
    }
}

#endif //OS_UTILITY_HPP
