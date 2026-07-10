#pragma once

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include "mystl/functional.hpp"
#include "mystl/iterator.hpp"
#include "mystl/utility.hpp"

namespace mystl {

template<typename Key, typename T, typename Compare = mystl::less<Key>>
class map {
private:
    using value_type = std::pair<const Key, T>;

    struct node {
        value_type value;
        node* left;
        node* right;
        node* parent;
        explicit node(const value_type& v) : value(v), left(nullptr), right(nullptr), parent(nullptr) {}
    };

public:
    class iterator {
    public:
        using iterator_category = mystl::bidirectional_iterator_tag;
        using value_type = std::pair<const Key, T>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        iterator() : cur_(nullptr), tree_(nullptr) {}
        iterator(node* n, const map* t) : cur_(n), tree_(t) {}

        reference operator*() const { return cur_->value; }
        pointer operator->() const { return mystl::addressof(cur_->value); }

        iterator& operator++() { cur_ = tree_->successor(cur_); return *this; }
        iterator operator++(int) { iterator tmp(*this); ++(*this); return tmp; }
        iterator& operator--() { cur_ = cur_ ? tree_->predecessor(cur_) : tree_->max_node(tree_->root_); return *this; }
        iterator operator--(int) { iterator tmp(*this); --(*this); return tmp; }

        friend bool operator==(const iterator& a, const iterator& b) { return a.cur_ == b.cur_; }
        friend bool operator!=(const iterator& a, const iterator& b) { return !(a == b); }

    private:
        node* cur_;
        const map* tree_;
        friend class map;
    };

    map() = default;
    map(std::initializer_list<value_type> init) { for (const auto& v : init) insert(v); }
    ~map() { clear(); }

    map(const map& other) { for (auto it = other.begin(); it != other.end(); ++it) insert(*it); }

    map(map&& other) noexcept : root_(other.root_), size_(other.size_), comp_(mystl::move(other.comp_)) {
        other.root_ = nullptr;
        other.size_ = 0;
    }

    map& operator=(const map& other) {
        if (this == &other) return *this;
        clear();
        for (auto it = other.begin(); it != other.end(); ++it) insert(*it);
        return *this;
    }

    map& operator=(map&& other) noexcept {
        if (this == &other) return *this;
        clear();
        root_ = other.root_;
        size_ = other.size_;
        comp_ = mystl::move(other.comp_);
        other.root_ = nullptr;
        other.size_ = 0;
        return *this;
    }

    bool empty() const noexcept { return size_ == 0; }
    std::size_t size() const noexcept { return size_; }

    iterator begin() const noexcept { return iterator(min_node(root_), this); }
    iterator end() const noexcept { return iterator(nullptr, this); }

    void clear() noexcept { clear_node(root_); root_ = nullptr; size_ = 0; }

    T& operator[](const Key& key) {
        auto res = insert(value_type(key, T{}));
        return res.first.cur_->value.second;
    }

    iterator find(const Key& key) const noexcept {
        node* cur = root_;
        while (cur) {
            if (comp_(key, cur->value.first)) cur = cur->left;
            else if (comp_(cur->value.first, key)) cur = cur->right;
            else return iterator(cur, this);
        }
        return end();
    }

    iterator lower_bound(const Key& key) const noexcept {
        node* cur = root_;
        node* candidate = nullptr;
        while (cur) {
            if (!comp_(cur->value.first, key)) {
                candidate = cur;
                cur = cur->left;
            } else {
                cur = cur->right;
            }
        }
        return iterator(candidate, this);
    }

    iterator upper_bound(const Key& key) const noexcept {
        node* cur = root_;
        node* candidate = nullptr;
        while (cur) {
            if (comp_(key, cur->value.first)) {
                candidate = cur;
                cur = cur->left;
            } else {
                cur = cur->right;
            }
        }
        return iterator(candidate, this);
    }

    std::pair<iterator, bool> insert(const value_type& value) {
        if (!root_) {
            root_ = new node(value);
            ++size_;
            return { iterator(root_, this), true };
        }
        node* cur = root_;
        node* parent = nullptr;
        while (cur) {
            parent = cur;
            if (comp_(value.first, cur->value.first)) cur = cur->left;
            else if (comp_(cur->value.first, value.first)) cur = cur->right;
            else return { iterator(cur, this), false };
        }
        node* n = new node(value);
        n->parent = parent;
        if (comp_(value.first, parent->value.first)) parent->left = n;
        else parent->right = n;
        ++size_;
        return { iterator(n, this), true };
    }

    std::size_t erase(const Key& key) {
        iterator it = find(key);
        if (it == end()) return 0;
        erase(it);
        return 1;
    }

    iterator erase(iterator pos) {
        node* n = pos.cur_;
        iterator next(successor(n), this);
        erase_node(n);
        --size_;
        return next;
    }

private:
    static node* min_node(node* n) {
        if (!n) return nullptr;
        while (n->left) n = n->left;
        return n;
    }

    static node* max_node(node* n) {
        if (!n) return nullptr;
        while (n->right) n = n->right;
        return n;
    }

    node* successor(node* n) const noexcept {
        if (!n) return nullptr;
        if (n->right) return min_node(n->right);
        node* p = n->parent;
        while (p && n == p->right) {
            n = p;
            p = p->parent;
        }
        return p;
    }

    node* predecessor(node* n) const noexcept {
        if (!n) return max_node(root_);
        if (n->left) return max_node(n->left);
        node* p = n->parent;
        while (p && n == p->left) {
            n = p;
            p = p->parent;
        }
        return p;
    }

    void replace_node(node* u, node* v) {
        if (!u->parent) root_ = v;
        else if (u == u->parent->left) u->parent->left = v;
        else u->parent->right = v;
        if (v) v->parent = u->parent;
    }

    void erase_node(node* z) {
        if (!z->left) {
            replace_node(z, z->right);
            delete z;
            return;
        }
        if (!z->right) {
            replace_node(z, z->left);
            delete z;
            return;
        }
        node* y = min_node(z->right);
        if (y->parent != z) {
            replace_node(y, y->right);
            y->right = z->right;
            if (y->right) y->right->parent = y;
        }
        replace_node(z, y);
        y->left = z->left;
        if (y->left) y->left->parent = y;
        delete z;
    }

    void clear_node(node* n) noexcept {
        if (!n) return;
        clear_node(n->left);
        clear_node(n->right);
        delete n;
    }

    node* root_ = nullptr;
    std::size_t size_ = 0;
    Compare comp_{};
};

} // namespace mystl
