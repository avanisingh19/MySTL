#pragma once

#include <type_traits>
#include <utility>

namespace mystl {

template<typename T>
constexpr std::remove_reference_t<T>&& move(T&& value) noexcept {
    return static_cast<std::remove_reference_t<T>&&>(value);
}

template<typename T>
constexpr T&& forward(std::remove_reference_t<T>& value) noexcept {
    return static_cast<T&&>(value);
}

template<typename T>
constexpr T&& forward(std::remove_reference_t<T>&& value) noexcept {
    static_assert(!std::is_lvalue_reference_v<T>, "bad mystl::forward call");
    return static_cast<T&&>(value);
}

template<typename T>
void swap(T& a, T& b) noexcept(noexcept(T(mystl::move(a))) && noexcept(a = mystl::move(b)) && noexcept(b = mystl::move(a))) {
    T temp = mystl::move(a);
    a = mystl::move(b);
    b = mystl::move(temp);
}

template<typename T>
constexpr T* addressof(T& value) noexcept {
    return std::addressof(value);
}

} // namespace mystl
