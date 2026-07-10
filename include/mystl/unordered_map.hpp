#pragma once

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <utility>
#include "mystl/iterator.hpp"
#include "mystl/utility.hpp"

namespace mystl {

template<typename Key, typename T, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
class unordered_map {
private:
    using value_type = std::pair<const Key, T>;

    struct node {
        value_type value;
        node* next;
        explicit node(const value_type& v) : value(v), next(nullptr) {}
    };

public:
    class iterator {
    public:
        using iterator_category = mystl::forward_iterator_tag;
        using value_type = std::pair<const Key, T>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        iterator() : map_(nullptr), bucket_(0), cur_(nullptr) {}
        iterator(const unordered_map* m, std::size_t b, node* n) : map_(m), bucket_(b), cur_(n) {}

        reference operator*() const { return cur_->value; }
        pointer operator->() const { return mystl::addressof(cur_->value); }

        iterator& operator++() { advance(); return *this; }
        iterator operator++(int) { iterator tmp(*this); advance(); return tmp; }

        friend bool operator==(const iterator& a, const iterator& b) { return a.cur_ == b.cur_ && a.bucket_ == b.bucket_ && a.map_ == b.map_; }
        friend bool operator!=(const iterator& a, const iterator& b) { return !(a == b); }

    private:
        void advance() {
            if (!map_ || !cur_) return;
            if (cur_->next) {
                cur_ = cur_->next;
                return;
            }
            ++bucket_;
            while (bucket_ < map_->bucket_count_ && map_->buckets_[bucket_] == nullptr) ++bucket_;
            cur_ = (bucket_ < map_->bucket_count_) ? map_->buckets_[bucket_] : nullptr;
        }

        const unordered_map* map_;
        std::size_t bucket_;
        node* cur_;
        friend class unordered_map;
    };

    unordered_map() : bucket_count_(8), size_(0), buckets_(new node*[bucket_count_]()), hash_(), eq_() {}
    unordered_map(std::initializer_list<value_type> init) : unordered_map() { for (const auto& v : init) insert(v); }
    ~unordered_map() { clear(); delete[] buckets_; }

    unordered_map(const unordered_map& other) : unordered_map() {
        for (auto it = other.begin(); it != other.end(); ++it) insert(*it);
    }

    unordered_map(unordered_map&& other) noexcept
        : bucket_count_(other.bucket_count_), size_(other.size_), buckets_(other.buckets_), hash_(mystl::move(other.hash_)), eq_(mystl::move(other.eq_)) {
        other.bucket_count_ = 0;
        other.size_ = 0;
        other.buckets_ = nullptr;
    }

    unordered_map& operator=(const unordered_map& other) {
        if (this == &other) return *this;
        clear();
        if (bucket_count_ != other.bucket_count_) {
            delete[] buckets_;
            bucket_count_ = other.bucket_count_;
            buckets_ = new node*[bucket_count_]();
        }
        for (auto it = other.begin(); it != other.end(); ++it) insert(*it);
        return *this;
    }

    unordered_map& operator=(unordered_map&& other) noexcept {
        if (this == &other) return *this;
        clear();
        delete[] buckets_;
        bucket_count_ = other.bucket_count_;
        size_ = other.size_;
        buckets_ = other.buckets_;
        hash_ = mystl::move(other.hash_);
        eq_ = mystl::move(other.eq_);
        other.bucket_count_ = 0;
        other.size_ = 0;
        other.buckets_ = nullptr;
        return *this;
    }

    bool empty() const noexcept { return size_ == 0; }
    std::size_t size() const noexcept { return size_; }

    iterator begin() const noexcept {
        for (std::size_t i = 0; i < bucket_count_; ++i) {
            if (buckets_[i]) return iterator(this, i, buckets_[i]);
        }
        return end();
    }

    iterator end() const noexcept { return iterator(this, bucket_count_, nullptr); }

    void clear() noexcept {
        if (!buckets_) return;
        for (std::size_t i = 0; i < bucket_count_; ++i) {
            node* cur = buckets_[i];
            while (cur) {
                node* next = cur->next;
                delete cur;
                cur = next;
            }
            buckets_[i] = nullptr;
        }
        size_ = 0;
    }

    std::pair<iterator, bool> insert(const value_type& value) {
        maybe_rehash();
        std::size_t idx = bucket_index(value.first);
        node* cur = buckets_[idx];
        while (cur) {
            if (eq_(cur->value.first, value.first)) return { iterator(this, idx, cur), false };
            cur = cur->next;
        }
        node* n = new node(value);
        n->next = buckets_[idx];
        buckets_[idx] = n;
        ++size_;
        return { iterator(this, idx, n), true };
    }

    iterator find(const Key& key) const noexcept {
        std::size_t idx = bucket_index(key);
        if (idx >= bucket_count_) return end();
        node* cur = buckets_[idx];
        while (cur) {
            if (eq_(cur->value.first, key)) return iterator(this, idx, cur);
            cur = cur->next;
        }
        return end();
    }

    T& operator[](const Key& key) {
        auto res = insert(value_type(key, T{}));
        return res.first.cur_->value.second;
    }

    std::size_t erase(const Key& key) {
        std::size_t idx = bucket_index(key);
        node* cur = buckets_[idx];
        node* prev = nullptr;
        while (cur) {
            if (eq_(cur->value.first, key)) {
                if (prev) prev->next = cur->next;
                else buckets_[idx] = cur->next;
                delete cur;
                --size_;
                return 1;
            }
            prev = cur;
            cur = cur->next;
        }
        return 0;
    }

    void rehash(std::size_t new_count) {
        if (new_count < 1) new_count = 1;
        node** new_buckets = new node*[new_count]();
        node** old_buckets = buckets_;
        std::size_t old_count = bucket_count_;
        buckets_ = new_buckets;
        bucket_count_ = new_count;
        size_ = 0;
        for (std::size_t i = 0; i < old_count; ++i) {
            node* cur = old_buckets[i];
            while (cur) {
                insert(cur->value);
                node* next = cur->next;
                delete cur;
                cur = next;
            }
        }
        delete[] old_buckets;
    }

private:
    std::size_t bucket_index(const Key& key) const noexcept {
        return bucket_count_ ? (hash_(key) % bucket_count_) : 0;
    }

    void maybe_rehash() {
        if (bucket_count_ == 0) {
            bucket_count_ = 8;
            buckets_ = new node*[bucket_count_]();
        }
        if ((size_ + 1) * 10 > bucket_count_ * 7) rehash(bucket_count_ * 2);
    }

    std::size_t bucket_count_;
    std::size_t size_;
    node** buckets_;
    Hash hash_;
    KeyEqual eq_;
};

} // namespace mystl
