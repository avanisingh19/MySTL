#pragma once

#include <cstddef>
#include <initializer_list>
#include <utility>
#include "mystl/functional.hpp"
#include "mystl/iterator.hpp"
#include "mystl/utility.hpp"

namespace mystl {

template<typename Key, typename Compare = mystl::less<Key>>
class set {
private:
    struct node {
        Key value;
        node* left;
        node* right;
        node* parent;
        explicit node(const Key& v) : value(v), left(nullptr), right(nullptr), parent(nullptr) {}
    };

public:
    class iterator {
    public:
        using iterator_category = mystl::bidirectional_iterator_tag;
        using value_type = Key;
        using difference_type = std::ptrdiff_t;
        using pointer = const Key*;
        using reference = const Key&;

        iterator() : cur_(nullptr), tree_(nullptr) {}
        iterator(node* n, const set* t) : cur_(n), tree_(t) {}

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
        const set* tree_;
        friend class set;
    };

    set() = default;
    set(std::initializer_list<Key> init) { for (const auto& v : init) insert(v); }
    ~set() { clear(); }

    set(const set& other) { for (auto it = other.begin(); it != other.end(); ++it) insert(*it); }

    set(set&& other) noexcept : root_(other.root_), size_(other.size_), comp_(mystl::move(other.comp_)) {
        other.root_ = nullptr;
        other.size_ = 0;
    }

    set& operator=(const set& other) {
        if (this == &other) return *this;
        clear();
        for (auto it = other.begin(); it != other.end(); ++it) insert(*it);
        return *this;
    }

    set& operator=(set&& other) noexcept {
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

    std::pair<iterator, bool> insert(const Key& value) {
        if (!root_) {
            root_ = new node(value);
            ++size_;
            return { iterator(root_, this), true };
        }
        node* cur = root_;
        node* parent = nullptr;
        while (cur) {
            parent = cur;
            if (comp_(value, cur->value)) cur = cur->left;
            else if (comp_(cur->value, value)) cur = cur->right;
            else return { iterator(cur, this), false };
        }
        node* n = new node(value);
        n->parent = parent;
        if (comp_(value, parent->value)) parent->left = n;
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

    iterator find(const Key& key) const noexcept {
        node* cur = root_;
        while (cur) {
            if (comp_(key, cur->value)) cur = cur->left;
            else if (comp_(cur->value, key)) cur = cur->right;
            else return iterator(cur, this);
        }
        return end();
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
