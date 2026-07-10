#pragma once

#include <cstddef>
#include <new>
#include <utility>
#include "mystl/utility.hpp"

namespace mystl {

template<typename T>
class allocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;

    constexpr allocator() noexcept = default;

    [[nodiscard]] pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }

    void deallocate(pointer p, size_type = 0) noexcept {
        ::operator delete(p);
    }

    template<typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        ::new ((void*)p) U(mystl::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U* p) noexcept {
        p->~U();
    }
};

} // namespace mystl
