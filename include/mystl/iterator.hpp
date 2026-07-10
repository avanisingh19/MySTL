#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>
#include "mystl/utility.hpp"

namespace mystl {

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};

template<typename Iterator>
struct iterator_traits {
    using difference_type = typename Iterator::difference_type;
    using value_type = typename Iterator::value_type;
    using pointer = typename Iterator::pointer;
    using reference = typename Iterator::reference;
    using iterator_category = typename Iterator::iterator_category;
};

template<typename T>
struct iterator_traits<T*> {
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = random_access_iterator_tag;
};

template<typename T>
struct iterator_traits<const T*> {
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = const T*;
    using reference = const T&;
    using iterator_category = random_access_iterator_tag;
};

template<typename InputIt>
constexpr typename iterator_traits<InputIt>::difference_type distance(InputIt first, InputIt last) {
    typename iterator_traits<InputIt>::difference_type n = 0;
    for (; first != last; ++first) ++n;
    return n;
}

template<typename InputIt, typename Distance>
constexpr void advance(InputIt& it, Distance n) {
    if constexpr (std::is_same_v<typename iterator_traits<InputIt>::iterator_category, random_access_iterator_tag>) {
        it += n;
    } else {
        while (n-- > 0) ++it;
    }
}

template<typename It>
class reverse_iterator {
public:
    using iterator_type = It;
    using iterator_category = typename iterator_traits<It>::iterator_category;
    using value_type = typename iterator_traits<It>::value_type;
    using difference_type = typename iterator_traits<It>::difference_type;
    using pointer = typename iterator_traits<It>::pointer;
    using reference = typename iterator_traits<It>::reference;

    reverse_iterator() : current_() {}
    explicit reverse_iterator(It it) : current_(it) {}

    It base() const { return current_; }

    reference operator*() const {
        It tmp = current_;
        --tmp;
        return *tmp;
    }

    pointer operator->() const { return mystl::addressof(operator*()); }

    reverse_iterator& operator++() { --current_; return *this; }
    reverse_iterator operator++(int) { reverse_iterator tmp(*this); --current_; return tmp; }
    reverse_iterator& operator--() { ++current_; return *this; }
    reverse_iterator operator--(int) { reverse_iterator tmp(*this); ++current_; return tmp; }

    friend bool operator==(const reverse_iterator& a, const reverse_iterator& b) { return a.current_ == b.current_; }
    friend bool operator!=(const reverse_iterator& a, const reverse_iterator& b) { return !(a == b); }

private:
    It current_;
};

template<typename It>
constexpr void iter_swap(It a, It b) {
    mystl::swap(*a, *b);
}

} // namespace mystl
