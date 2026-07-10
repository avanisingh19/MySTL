#pragma once

#include <cstddef>
#include <initializer_list>
#include "mystl/list.hpp"

namespace mystl {

template<typename T>
class deque {
public:
    using value_type = T;
    using iterator = typename list<T>::iterator;

    deque() = default;
    deque(std::initializer_list<T> init) : impl_(init) {}

    bool empty() const noexcept { return impl_.empty(); }
    std::size_t size() const noexcept { return impl_.size(); }

    T& front() { return impl_.front(); }
    const T& front() const { return impl_.front(); }

    T& back() { return impl_.back(); }
    const T& back() const { return impl_.back(); }

    iterator begin() noexcept { return impl_.begin(); }
    iterator end() noexcept { return impl_.end(); }

    void push_front(const T& v) { impl_.push_front(v); }
    void push_front(T&& v) { impl_.push_front(mystl::move(v)); }

    void push_back(const T& v) { impl_.push_back(v); }
    void push_back(T&& v) { impl_.push_back(mystl::move(v)); }

    void pop_front() { impl_.pop_front(); }
    void pop_back() { impl_.pop_back(); }

    void clear() noexcept { impl_.clear(); }

private:
    list<T> impl_;
};

} // namespace mystl
