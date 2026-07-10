#pragma once

#include <cstddef>
#include <initializer_list>
#include "mystl/deque.hpp"

namespace mystl {

template<typename T, typename Container = mystl::deque<T>>
class queue {
public:
    queue() = default;
    queue(std::initializer_list<T> init) : c_(init) {}

    bool empty() const noexcept { return c_.empty(); }
    std::size_t size() const noexcept { return c_.size(); }

    T& front() { return c_.front(); }
    const T& front() const { return c_.front(); }

    T& back() { return c_.back(); }
    const T& back() const { return c_.back(); }

    void push(const T& v) { c_.push_back(v); }
    void push(T&& v) { c_.push_back(mystl::move(v)); }

    void pop() { c_.pop_front(); }

private:
    Container c_;
};

} // namespace mystl
