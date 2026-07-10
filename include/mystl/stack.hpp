#pragma once

#include <cstddef>
#include <initializer_list>
#include "mystl/vector.hpp"

namespace mystl {

template<typename T, typename Container = mystl::vector<T>>
class stack {
public:
    stack() = default;
    stack(std::initializer_list<T> init) : c_(init) {}

    bool empty() const noexcept { return c_.empty(); }
    std::size_t size() const noexcept { return c_.size(); }

    T& top() { return c_.back(); }
    const T& top() const { return c_.back(); }

    void push(const T& v) { c_.push_back(v); }
    void push(T&& v) { c_.push_back(mystl::move(v)); }

    void pop() { c_.pop_back(); }

private:
    Container c_;
};

} // namespace mystl
