#pragma once

#include <cstddef>
#include <initializer_list>
#include <utility>
#include "mystl/iterator.hpp"
#include "mystl/utility.hpp"

namespace mystl {

template<typename T>
class list {
private:
    struct node_base {
        node_base* prev;
        node_base* next;
        node_base() : prev(this), next(this) {}
    };

    struct node : node_base {
        T value;
        template<typename U>
        explicit node(U&& v) : node_base(), value(mystl::forward<U>(v)) {}
    };

public:
    class iterator {
    public:
        using iterator_category = mystl::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator() : node_(nullptr) {}
        explicit iterator(node_base* n) : node_(n) {}

        reference operator*() const { return static_cast<node*>(node_)->value; }
        pointer operator->() const { return mystl::addressof(operator*()); }

        iterator& operator++() { node_ = node_->next; return *this; }
        iterator operator++(int) { iterator tmp(*this); ++(*this); return tmp; }
        iterator& operator--() { node_ = node_->prev; return *this; }
        iterator operator--(int) { iterator tmp(*this); --(*this); return tmp; }

        friend bool operator==(const iterator& a, const iterator& b) { return a.node_ == b.node_; }
        friend bool operator!=(const iterator& a, const iterator& b) { return !(a == b); }

        node_base* base() const { return node_; }

    private:
        node_base* node_;
        friend class list;
    };

    list() : size_(0) {}
    list(std::initializer_list<T> init) : list() { for (const auto& v : init) push_back(v); }

    list(const list& other) : list() { for (auto it = other.begin(); it != other.end(); ++it) push_back(*it); }

    list(list&& other) noexcept : list() { splice_all_from(other); }

    ~list() { clear(); }

    list& operator=(const list& other) {
        if (this == &other) return *this;
        clear();
        for (auto it = other.begin(); it != other.end(); ++it) push_back(*it);
        return *this;
    }

    list& operator=(list&& other) noexcept {
        if (this == &other) return *this;
        clear();
        splice_all_from(other);
        return *this;
    }

    bool empty() const noexcept { return size_ == 0; }
    std::size_t size() const noexcept { return size_; }

    iterator begin() noexcept { return iterator(sentinel_.next); }
    iterator end() noexcept { return iterator(&sentinel_); }
    iterator begin() const noexcept { return iterator(sentinel_.next); }
    iterator end() const noexcept { return iterator(const_cast<node_base*>(&sentinel_)); }

    T& front() { return static_cast<node*>(sentinel_.next)->value; }
    const T& front() const { return static_cast<node*>(sentinel_.next)->value; }

    T& back() { return static_cast<node*>(sentinel_.prev)->value; }
    const T& back() const { return static_cast<node*>(sentinel_.prev)->value; }

    void clear() noexcept {
        while (!empty()) pop_front();
    }

    template<typename... Args>
    iterator emplace(iterator pos, Args&&... args) {
        node* n = new node(T(mystl::forward<Args>(args)...));
        link_before(pos.base(), n);
        ++size_;
        return iterator(n);
    }

    iterator insert(iterator pos, const T& value) { return emplace(pos, value); }
    iterator insert(iterator pos, T&& value) { return emplace(pos, mystl::move(value)); }

    template<typename... Args>
    void emplace_back(Args&&... args) { emplace(end(), mystl::forward<Args>(args)...); }
    template<typename... Args>
    void emplace_front(Args&&... args) { emplace(begin(), mystl::forward<Args>(args)...); }

    void push_back(const T& value) { emplace_back(value); }
    void push_back(T&& value) { emplace_back(mystl::move(value)); }
    void push_front(const T& value) { emplace_front(value); }
    void push_front(T&& value) { emplace_front(mystl::move(value)); }

    void pop_back() {
        if (empty()) return;
        erase(iterator(sentinel_.prev));
    }

    void pop_front() {
        if (empty()) return;
        erase(begin());
    }

    iterator erase(iterator pos) {
        node_base* b = pos.base();
        iterator next(b->next);
        unlink(b);
        delete static_cast<node*>(b);
        --size_;
        return next;
    }

private:
    void link_before(node_base* pos, node* n) {
        n->next = pos;
        n->prev = pos->prev;
        pos->prev->next = n;
        pos->prev = n;
    }

    void unlink(node_base* n) {
        n->prev->next = n->next;
        n->next->prev = n->prev;
    }

    void splice_all_from(list& other) noexcept {
        if (other.empty()) return;
        sentinel_.next = other.sentinel_.next;
        sentinel_.prev = other.sentinel_.prev;
        sentinel_.next->prev = &sentinel_;
        sentinel_.prev->next = &sentinel_;
        size_ = other.size_;
        other.sentinel_.next = &other.sentinel_;
        other.sentinel_.prev = &other.sentinel_;
        other.size_ = 0;
    }

    node_base sentinel_;
    std::size_t size_;
};

} // namespace mystl
