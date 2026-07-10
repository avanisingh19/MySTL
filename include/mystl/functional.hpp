#pragma once

namespace mystl {

template<typename T = void>
struct less {
    constexpr bool operator()(const T& a, const T& b) const { return a < b; }
};

template<typename T = void>
struct greater {
    constexpr bool operator()(const T& a, const T& b) const { return a > b; }
};

template<typename T = void>
struct equal_to {
    constexpr bool operator()(const T& a, const T& b) const { return a == b; }
};

template<typename T = void>
struct not_equal_to {
    constexpr bool operator()(const T& a, const T& b) const { return a != b; }
};

template<typename T = void>
struct plus {
    constexpr T operator()(const T& a, const T& b) const { return a + b; }
};

template<typename T = void>
struct minus {
    constexpr T operator()(const T& a, const T& b) const { return a - b; }
};

template<typename T = void>
struct multiplies {
    constexpr T operator()(const T& a, const T& b) const { return a * b; }
};

template<typename T = void>
struct divides {
    constexpr T operator()(const T& a, const T& b) const { return a / b; }
};

} // namespace mystl
