//
// Created by wmj on 3/20/20.
//

#ifndef OS_TYPE_TRAITS_HPP
#define OS_TYPE_TRAITS_HPP

namespace std {
    template< class T > struct remove_reference      {typedef T type;};
    template< class T > struct remove_reference<T&>  {typedef T type;};
    template< class T > struct remove_reference<T&&> {typedef T type;};
    template< class T > using remove_reference_t = typename remove_reference<T>::type;

    template<bool B, class T = void> struct enable_if {};
    template<class T> struct enable_if<true, T> { typedef T type; };

    template<class T, T v>
    struct integral_constant {
        static constexpr T value = v;
        using value_type = T;
        using type = integral_constant; // 使用注入类名
        constexpr operator value_type() const noexcept { return value; }
        constexpr value_type operator()() const noexcept { return value; } // C++14 起
    };

    using false_type = integral_constant<bool, false>;
    using true_type  = integral_constant<bool, true>;

    template<class T> struct is_lvalue_reference     : std::false_type {};
    template<class T> struct is_lvalue_reference<T&> : std::true_type {};
}

#endif //OS_TYPE_TRAITS_HPP
