#pragma once

#include "mystl/functional.hpp"
#include "mystl/iterator.hpp"
#include "mystl/utility.hpp"

namespace mystl {

template<typename InputIt, typename T>
constexpr InputIt find(InputIt first, InputIt last, const T& value) {
    for (; first != last; ++first) {
        if (*first == value) return first;
    }
    return last;
}

template<typename InputIt, typename OutputIt>
constexpr OutputIt copy(InputIt first, InputIt last, OutputIt out) {
    for (; first != last; ++first, ++out) *out = *first;
    return out;
}

template<typename InputIt, typename OutputIt, typename UnaryOp>
constexpr OutputIt transform(InputIt first, InputIt last, OutputIt out, UnaryOp op) {
    for (; first != last; ++first, ++out) *out = op(*first);
    return out;
}

template<typename BidirIt>
constexpr void reverse(BidirIt first, BidirIt last) {
    while (first != last && first != --last) {
        mystl::iter_swap(first++, last);
    }
}

template<typename It, typename T, typename Compare = mystl::less<T>>
constexpr It lower_bound(It first, It last, const T& value, Compare comp = Compare{}) {
    auto count = mystl::distance(first, last);
    while (count > 0) {
        auto step = count / 2;
        It mid = first;
        mystl::advance(mid, step);
        if (comp(*mid, value)) {
            first = ++mid;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}

template<typename It, typename T, typename Compare = mystl::less<T>>
constexpr It upper_bound(It first, It last, const T& value, Compare comp = Compare{}) {
    auto count = mystl::distance(first, last);
    while (count > 0) {
        auto step = count / 2;
        It mid = first;
        mystl::advance(mid, step);
        if (!comp(value, *mid)) {
            first = ++mid;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}

template<typename It, typename T, typename Compare = mystl::less<T>>
constexpr bool binary_search(It first, It last, const T& value, Compare comp = Compare{}) {
    It it = mystl::lower_bound(first, last, value, comp);
    return it != last && !comp(value, *it) && !comp(*it, value);
}

namespace detail {
template<typename RandomIt, typename Compare>
void quick_sort(RandomIt first, RandomIt last, Compare comp) {
    auto n = last - first;
    if (n <= 1) return;

    RandomIt left = first;
    RandomIt right = last - 1;
    auto pivot = *(first + n / 2);

    while (left <= right) {
        while (comp(*left, pivot)) ++left;
        while (comp(pivot, *right)) --right;
        if (left <= right) {
            mystl::iter_swap(left, right);
            ++left;
            --right;
        }
    }

    if (first < right + 1) quick_sort(first, right + 1, comp);
    if (left < last) quick_sort(left, last, comp);
}
} // namespace detail

template<typename RandomIt, typename Compare = mystl::less<typename iterator_traits<RandomIt>::value_type>>
constexpr void sort(RandomIt first, RandomIt last, Compare comp = Compare{}) {
    if (first == last) return;
    detail::quick_sort(first, last, comp);
}

} // namespace mystl
