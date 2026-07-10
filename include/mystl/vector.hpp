#pragma once

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include "mystl/allocator.hpp"
#include "mystl/iterator.hpp"
#include "mystl/utility.hpp"

namespace mystl {

template<typename T, typename Alloc = mystl::allocator<T>>
class vector {
public:
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

    vector() noexcept : data_(nullptr), size_(0), capacity_(0), alloc_() {}

    explicit vector(size_type count, const T& value = T()) : vector() {
        resize(count, value);
    }

    vector(std::initializer_list<T> init) : vector() {
        reserve(init.size());
        for (const auto& v : init) emplace_back(v);
    }

    vector(const vector& other) : vector() {
        reserve(other.size_);
        for (size_type i = 0; i < other.size_; ++i) alloc_.construct(data_ + i, other.data_[i]);
        size_ = other.size_;
    }

    vector(vector&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_), alloc_(mystl::move(other.alloc_)) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    ~vector() {
        clear();
        alloc_.deallocate(data_, capacity_);
    }

    vector& operator=(const vector& other) {
        if (this == &other) return *this;
        vector tmp(other);
        swap(tmp);
        return *this;
    }

    vector& operator=(vector&& other) noexcept {
        if (this == &other) return *this;
        clear();
        alloc_.deallocate(data_, capacity_);
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        alloc_ = mystl::move(other.alloc_);
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        return *this;
    }

    vector& operator=(std::initializer_list<T> init) {
        clear();
        reserve(init.size());
        for (const auto& v : init) emplace_back(v);
        return *this;
    }

    void swap(vector& other) noexcept {
        mystl::swap(data_, other.data_);
        mystl::swap(size_, other.size_);
        mystl::swap(capacity_, other.capacity_);
        mystl::swap(alloc_, other.alloc_);
    }

    size_type size() const noexcept { return size_; }
    size_type capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return size_ == 0; }

    pointer data() noexcept { return data_; }
    const_pointer data() const noexcept { return data_; }

    reference operator[](size_type pos) { return data_[pos]; }
    const_reference operator[](size_type pos) const { return data_[pos]; }

    reference at(size_type pos) {
        if (pos >= size_) throw std::out_of_range("mystl::vector::at");
        return data_[pos];
    }

    const_reference at(size_type pos) const {
        if (pos >= size_) throw std::out_of_range("mystl::vector::at");
        return data_[pos];
    }

    reference front() { return data_[0]; }
    const_reference front() const { return data_[0]; }

    reference back() { return data_[size_ - 1]; }
    const_reference back() const { return data_[size_ - 1]; }

    iterator begin() noexcept { return data_; }
    const_iterator begin() const noexcept { return data_; }
    const_iterator cbegin() const noexcept { return data_; }

    iterator end() noexcept { return data_ ? data_ + size_ : data_; }
    const_iterator end() const noexcept { return data_ ? data_ + size_ : data_; }
    const_iterator cend() const noexcept { return data_ ? data_ + size_ : data_; }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    void clear() noexcept {
        for (size_type i = 0; i < size_; ++i) alloc_.destroy(data_ + i);
        size_ = 0;
    }

    void reserve(size_type new_cap) {
        if (new_cap <= capacity_) return;
        pointer new_data = alloc_.allocate(new_cap);
        size_type i = 0;
        try {
            for (; i < size_; ++i) alloc_.construct(new_data + i, mystl::move(data_[i]));
        } catch (...) {
            for (size_type j = 0; j < i; ++j) alloc_.destroy(new_data + j);
            alloc_.deallocate(new_data, new_cap);
            throw;
        }
        clear();
        alloc_.deallocate(data_, capacity_);
        data_ = new_data;
        capacity_ = new_cap;
        size_ = i;
    }

    void resize(size_type count, const T& value = T()) {
        if (count < size_) {
            for (size_type i = count; i < size_; ++i) alloc_.destroy(data_ + i);
            size_ = count;
            return;
        }
        if (count > capacity_) reserve(grow_capacity(count));
        for (size_type i = size_; i < count; ++i) alloc_.construct(data_ + i, value);
        size_ = count;
    }

    template<typename... Args>
    reference emplace_back(Args&&... args) {
        if (size_ == capacity_) reserve(grow_capacity(size_ + 1));
        alloc_.construct(data_ + size_, mystl::forward<Args>(args)...);
        return data_[size_++];
    }

    void push_back(const T& value) { emplace_back(value); }
    void push_back(T&& value) { emplace_back(mystl::move(value)); }

    void pop_back() {
        if (size_ == 0) return;
        --size_;
        alloc_.destroy(data_ + size_);
    }

    iterator insert(iterator pos, const T& value) { return emplace(pos, value); }
    iterator insert(iterator pos, T&& value) { return emplace(pos, mystl::move(value)); }

    iterator erase(iterator pos) {
        size_type idx = static_cast<size_type>(pos - begin());
        if (idx >= size_) return end();
        for (size_type i = idx; i + 1 < size_; ++i) data_[i] = mystl::move(data_[i + 1]);
        pop_back();
        return begin() + idx;
    }

private:
    template<typename U>
    iterator emplace(iterator pos, U&& value) {
        size_type idx = static_cast<size_type>(pos - begin());
        if (idx > size_) idx = size_;
        if (size_ == capacity_) reserve(grow_capacity(size_ + 1));
        if (idx == size_) {
            alloc_.construct(data_ + size_, mystl::forward<U>(value));
        } else {
            alloc_.construct(data_ + size_, mystl::move(data_[size_ - 1]));
            for (size_type i = size_ - 1; i > idx; --i) data_[i] = mystl::move(data_[i - 1]);
            data_[idx] = mystl::forward<U>(value);
        }
        ++size_;
        return begin() + idx;
    }

    static size_type grow_capacity(size_type min_cap) {
        size_type cap = 1;
        while (cap < min_cap) cap *= 2;
        return cap;
    }

    pointer data_;
    size_type size_;
    size_type capacity_;
    allocator_type alloc_;
};

template<typename T, typename Alloc>
inline void swap(vector<T, Alloc>& a, vector<T, Alloc>& b) noexcept { a.swap(b); }

} // namespace mystl
